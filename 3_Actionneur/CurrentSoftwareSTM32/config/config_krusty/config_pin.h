/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des pins
 *  Auteur : Jacen, Alexis
 */

#ifndef KRUSTY_CONFIG_PIN_H
#define KRUSTY_CONFIG_PIN_H

/* Config BALLLAUNCHER */
	#define BALLLAUNCHER_DCMOTOR_ID              0			//Doit être unique !
	#define BALLLAUNCHER_DCMOTOR_PWM_NUM         4
	#define BALLLAUNCHER_DCMOTOR_PORT_WAY        GPIOC->ODR
	#define BALLLAUNCHER_DCMOTOR_PORT_WAY_BIT    12
//	#define BALLLAUNCHER_HALLSENSOR_INT_FLAG     IFS1bits.INT1IF
//	#define BALLLAUNCHER_HALLSENSOR_INT_PRIORITY IPC4bits.INT1IP
//	#define BALLLAUNCHER_HALLSENSOR_INT_ENABLE   IEC1bits.INT1IE
//	#define BALLLAUNCHER_HALLSENSOR_INT_EDGE     INTCON2bits.INT1EP
//	#define BALLLAUNCHER_HALLSENSOR_INT_ISR      __attribute__((interrupt, no_auto_psv)) _INT1Interrupt

/* Config PLATE */
	#define PLATE_DCMOTOR_ID                     1      //Doit être unique !
	#define PLATE_DCMOTOR_PWM_NUM                3
	#define PLATE_DCMOTOR_PORT_WAY               GPIOC->ODR
	#define PLATE_DCMOTOR_PORT_WAY_BIT           11
	#define PLATE_ROTATION_POTAR_ADC_ID          AN9_ID
	#define PLATE_PLIER_AX12_ID                  3

/* Config BALLSORTER */
	#define BALLSORTER_AX12_ID                   5
	#define BALLSORTER_SENSOR_PIN_WHITE_CHERRY   GPIOB->ODR12  //Si utilisation du gros sick (capteur couleur, voir BALLSORTER_USE_SICK_SENSOR dans KBall_sorter_config.h)
	#define BALLSORTER_SENSOR_PIN_RED_CHERRY     GPIOB->ODR13
	#define BALLSORTER_SENSOR_PIN_BLUE_CHERRY    GPIOB->ODR14
	#define BALLSORTER_SENSOR_PIN                GPIOB->ODR12  //Si utilisation du sick wt100
	#define BALLSORTER_SENSOR_DETECTED_LEVEL     0      //Niveau de BALLSORTER_SENSOR_PIN quand une cerise blanche est détectée

/* Config LIFT */
	#define LIFT_LEFT_DCMOTOR_ID                     2    //Doit être unique !
	#define LIFT_LEFT_DCMOTOR_PWM_NUM                1
	#define LIFT_LEFT_DCMOTOR_PORT_WAY               GPIOA->ODR
	#define LIFT_LEFT_DCMOTOR_PORT_WAY_BIT           8
	#define LIFT_LEFT_TRANSLATION_POTAR_ADC_ID       AN3_ID
	#define LIFT_LEFT_PLIER_AX12_ID                  4


	#define LIFT_RIGHT_DCMOTOR_ID                     3      //Doit être unique !
	#define LIFT_RIGHT_DCMOTOR_PWM_NUM                2
	#define LIFT_RIGHT_DCMOTOR_PORT_WAY               GPIOC->ODR
	#define LIFT_RIGHT_DCMOTOR_PORT_WAY_BIT           10
	#define LIFT_RIGHT_TRANSLATION_POTAR_ADC_ID       AN2_ID
	#define LIFT_RIGHT_PLIER_AX12_ID                  1

#endif /* KRUSTY_CONFIG_PIN_H */
