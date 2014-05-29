/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id: Arm_config.h 1983 2014-04-05 09:00:40Z aguilmet $
 *
 *  Package : Carte Actionneur
 *  Description : Configuration de la gestion du bras
 *  Auteur : Alexis
 *  Robot : Pierre & Guy
 */

#ifndef ARM_CONFIG_H
#define	ARM_CONFIG_H

#include "config_pin.h"

#define ARM_ACT_UPDOWN_ID_POTAR_ADC_ID           ADC_11

//#define ARM_ACT_UPDOWN                           0
#define ARM_ACT_UPDOWN_DOUBLE_PID				 TRUE
#define ARM_ACT_UPDOWN_ASSER_KP                  3000
#define ARM_ACT_UPDOWN_ASSER_KI                  1000
#define ARM_ACT_UPDOWN_ASSER_KD                  0
#define ARM_ACT_UPDOWN_ASSER_KP2                 3000
#define ARM_ACT_UPDOWN_ASSER_KI2                 1000
#define ARM_ACT_UPDOWN_ASSER_KD2                 0
#define ARM_ACT_UPDOWN_ASSER_TIMEOUT             5000
#define ARM_ACT_UPDOWN_ASSER_POS_EPSILON         20
#define ARM_ACT_UPDOWN_ASSER_POS_LARGE_EPSILON   300 //arbitraire, a changer
#define ARM_ACT_UPDOWN_PWM_NUM                   1
#define ARM_ACT_UPDOWN_PORT_WAY                  GPIOA
#define ARM_ACT_UPDOWN_PORT_WAY_BIT              8
#define ARM_ACT_UPDOWN_MAX_PWM_WAY0              80
#define ARM_ACT_UPDOWN_MAX_PWM_WAY1              80
#define ARM_ACT_UPDOWN_MAX_VALUE				 -73
#define ARM_ACT_UPDOWN_MIN_VALUE				 -530


#define ARM_ACT_UPDOWN_RUSH_IN_FLOOR_PWM		30

//#define ARM_ACT_RX24                             1
#define ARM_ACT_RX24                             0		//non utilisé, mais utilisé comme prefix dans arm_data (les autres moteurs c'est pareil)
#define ARM_ACT_RX24_ASSER_TIMEOUT               50
#define ARM_ACT_RX24_ASSER_POS_EPSILON           5
#define ARM_ACT_RX24_ASSER_POS_LARGE_EPSILON     10 //arbitraire, a changer
#define ARM_ACT_RX24_MAX_TORQUE_PERCENT          50
#define ARM_ACT_RX24_MAX_VALUE					 300
#define ARM_ACT_RX24_MIN_VALUE					 0

#define ARM_ACT_AX12_MID                         2
#define ARM_ACT_AX12_MID_ASSER_TIMEOUT           50
#define ARM_ACT_AX12_MID_ASSER_POS_EPSILON       5
#define ARM_ACT_AX12_MID_ASSER_POS_LARGE_EPSILON 10 //arbitraire, a changer
#define ARM_ACT_AX12_MID_MAX_TORQUE_PERCENT      75
#define ARM_ACT_AX12_MID_MAX_VALUE				 300
#define ARM_ACT_AX12_MID_MIN_VALUE				 0

#define ARM_ACT_AX12_TRIANGLE                             3
#define ARM_ACT_AX12_TRIANGLE_ASSER_TIMEOUT               50
#define ARM_ACT_AX12_TRIANGLE_ASSER_POS_EPSILON           5
#define ARM_ACT_AX12_TRIANGLE_ASSER_POS_LARGE_EPSILON     10 //arbitraire, a changer
#define ARM_ACT_AX12_TRIANGLE_MAX_TORQUE_PERCENT          70
#define ARM_ACT_AX12_TRIANGLE_MAX_VALUE					 300
#define ARM_ACT_AX12_TRIANGLE_MIN_VALUE					 0

#endif	/* ARM_CONFIG_H */

