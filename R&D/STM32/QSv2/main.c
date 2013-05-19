/*
 * main.c
 *
 *  Created on: 15 mai 2013
 *      Author: doc
 */


#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#define LED_GREEN      12
#define LED_ORANGE     13
#define LED_RED        14
#define LED_BLUE       15

int main()
{
    int pin = LED_ORANGE;
    uint32_t mode = GPIO_Mode_OUT << (pin * 2);
    uint32_t speed = GPIO_Speed_100MHz << (pin * 2);
    uint32_t type = GPIO_OType_PP << pin;
    uint32_t pullup = GPIO_PuPd_NOPULL << (pin * 2);
    //
    //  Initialise the peripheral clock.
    //
    RCC->AHB1ENR |= RCC_AHB1Periph_GPIOD;
    //
    //  Initilaise the GPIO port.
    //
    GPIOD->MODER |= mode;
    GPIOD->OSPEEDR |= speed;
    GPIOD->OTYPER |= type;
    GPIOD->PUPDR |= pullup;
    //
    //  Toggle the selected LED indefinitely.
    //
    int index;
    while (1)
    {
        GPIOD->BSRRL = (1 << pin);
        for (index = 0; index < 500000; index++);
        GPIOD->BSRRH = (1 << pin);
        for (index = 0; index < 500000; index++);
    }
}
