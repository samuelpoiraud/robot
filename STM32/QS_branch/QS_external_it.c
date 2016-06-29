/*
 *  Club Robot ESEO 2013 - 2014
 *
 *
 *  Fichier : QS_external_it.c
 *  Package : Qualité Soft
 *  Description : Gestion des interruptions externes
 *  Auteur : Alexis
 *  Version 20130929
 */

#include "QS_external_it.h"
#include "../stm32f4xx_hal/stm32f4xx_hal_gpio.h"


#ifdef USE_EXTERNAL_IT
	#warning "SOYEZ PRUDENT AVEC LES IT EXTERNES"

	static GPIO_TypeDef* EXTERNALIT_get_port(EXTERNALIT_port_e port);
	static Uint16 EXTERNALIT_get_pin(EXTERNALIT_pin_e pin);

	EXTERNALIT_callback_it_t callbacks[16];
	volatile static bool_e initialized = FALSE;

	void EXTERNALIT_init() {
		if (initialized)
			return;

		Uint8 i;

		for(i = 0; i < 16; i++) {
			callbacks[i] = NULL;
		}

		__HAL_RCC_SYSCFG_CLK_ENABLE();
		initialized = TRUE;
	}

	void EXTERNALIT_configure(EXTERNALIT_port_e port, EXTERNALIT_pin_e pin, EXTERNALIT_edge_e edge, EXTERNALIT_callback_it_t callback) {
		IRQn_Type irq;

		assert(pin < 16);

		EXTERNALIT_set_edge(port, pin, edge);

		callbacks[pin] = callback;

		if(pin < 5)
			irq = EXTI0_IRQn + pin;
		else if(pin < 10)
			irq = EXTI9_5_IRQn;
		else
			irq = EXTI15_10_IRQn;

		HAL_NVIC_SetPriority(irq, 0, 0);
		HAL_NVIC_EnableIRQ(irq);
	}

	void EXTERNALIT_set_edge(EXTERNALIT_port_e port, EXTERNALIT_pin_e pin, EXTERNALIT_edge_e edge) {
		GPIO_InitTypeDef GPIO_InitStructure;

		GPIO_InitStructure.Pin = EXTERNALIT_get_pin(pin);
		GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
		GPIO_InitStructure.Alternate = 0;
		GPIO_InitStructure.Pull = GPIO_NOPULL;

		switch(edge) {
			case EXTIT_Edge_Rising:
				GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;;
				break;

			case EXTIT_Edge_Falling:
				GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
				break;

			case EXTIT_Edge_Both:
				GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
				break;
		}

		HAL_GPIO_Init(EXTERNALIT_get_port(port), &GPIO_InitStructure);
	}

	void EXTERNALIT_set_priority(EXTERNALIT_pin_e pin, Uint8 priority) {
		IRQn_Type irq;

		if(pin < 5)
			irq = EXTI0_IRQn + pin;
		else if(pin < 10)
			irq = EXTI9_5_IRQn;
		else
			irq = EXTI15_10_IRQn;

		HAL_NVIC_SetPriority(irq, priority, 0);
		HAL_NVIC_EnableIRQ(irq);
	}

	void EXTERNALIT_disable(EXTERNALIT_port_e port, EXTERNALIT_pin_e pin) {
		HAL_GPIO_DeInit(EXTERNALIT_get_port(port),EXTERNALIT_get_pin(pin));
	}

	void EXTERNALIT_set_it_enabled(EXTERNALIT_pin_e pin, bool_e enabled) {
		IRQn_Type irq;

		if(pin < 5)
			irq = EXTI0_IRQn + pin;
		else if(pin < 10)
			irq = EXTI9_5_IRQn;
		else
			irq = EXTI15_10_IRQn;

		if(enabled)
			HAL_NVIC_EnableIRQ(irq);
		else
			HAL_NVIC_DisableIRQ(irq);
	}

	static GPIO_TypeDef* EXTERNALIT_get_port(EXTERNALIT_port_e port) {
		GPIO_TypeDef* gpio_port;
		switch(port){
			case EXTIT_GpioA : gpio_port = GPIOA; break;
			case EXTIT_GpioB : gpio_port = GPIOB; break;
			case EXTIT_GpioC : gpio_port = GPIOC; break;
			case EXTIT_GpioD : gpio_port = GPIOD; break;
			case EXTIT_GpioE : gpio_port = GPIOE; break;
			case EXTIT_GpioF : gpio_port = GPIOF; break;
			case EXTIT_GpioG : gpio_port = GPIOG; break;
		}
		return gpio_port;
	}

	static Uint16 EXTERNALIT_get_pin(EXTERNALIT_pin_e pin) {
		Uint16 gpio_pin;
		switch(pin){
			case EXTIT_Pin0  : gpio_pin = GPIO_PIN_0; break;
			case EXTIT_Pin1  : gpio_pin = GPIO_PIN_1; break;
			case EXTIT_Pin2  : gpio_pin = GPIO_PIN_2; break;
			case EXTIT_Pin3  : gpio_pin = GPIO_PIN_3; break;
			case EXTIT_Pin4  : gpio_pin = GPIO_PIN_4; break;
			case EXTIT_Pin5  : gpio_pin = GPIO_PIN_5; break;
			case EXTIT_Pin6  : gpio_pin = GPIO_PIN_6; break;
			case EXTIT_Pin7  : gpio_pin = GPIO_PIN_7; break;
			case EXTIT_Pin8  : gpio_pin = GPIO_PIN_8; break;
			case EXTIT_Pin9  : gpio_pin = GPIO_PIN_9; break;
			case EXTIT_Pin10 : gpio_pin = GPIO_PIN_10; break;
			case EXTIT_Pin11 : gpio_pin = GPIO_PIN_11; break;
			case EXTIT_Pin12 : gpio_pin = GPIO_PIN_12; break;
			case EXTIT_Pin13 : gpio_pin = GPIO_PIN_13; break;
			case EXTIT_Pin14 : gpio_pin = GPIO_PIN_14; break;
			case EXTIT_Pin15 : gpio_pin = GPIO_PIN_15; break;
		}
		return gpio_pin;
	}

	//Interrupts callbacks
	void HAL_GPIO_EXTI_Callback(Uint16 gpio_pin)
	{
		switch(gpio_pin){
			case GPIO_PIN_0  : callbacks[0](); break;
			case GPIO_PIN_1  : callbacks[1](); break;
			case GPIO_PIN_2  : callbacks[2](); break;
			case GPIO_PIN_3  : callbacks[3](); break;
			case GPIO_PIN_4  : callbacks[4](); break;
			case GPIO_PIN_5  : callbacks[5](); break;
			case GPIO_PIN_6  : callbacks[6](); break;
			case GPIO_PIN_7  : callbacks[7](); break;
			case GPIO_PIN_8  : callbacks[8](); break;
			case GPIO_PIN_9  : callbacks[9](); break;
			case GPIO_PIN_10 : callbacks[10](); break;
			case GPIO_PIN_11 : callbacks[11](); break;
			case GPIO_PIN_12 : callbacks[12](); break;
			case GPIO_PIN_13 : callbacks[13](); break;
			case GPIO_PIN_14 : callbacks[14](); break;
			case GPIO_PIN_15 : callbacks[15](); break;
		}
	}

#endif //USE_EXTERNAL_IT
