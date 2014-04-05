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

// ID des moteurs
#define ARM_ACT_UPDOWN_ID                        0
#define ARM_ACT_RX24_ID                          20
#define ARM_ACT_AX12_MID_ID                      2
#define ARM_ACT_AX12_TRIANGLE_ID                 6

#define ARM_ACT_UPDOWN_ID_POTAR_ADC_ID           ADC_11

//#define ARM_ACT_UPDOWN                           0
#define ARM_ACT_UPDOWN_ASSER_KP                  100
#define ARM_ACT_UPDOWN_ASSER_KI                  0
#define ARM_ACT_UPDOWN_ASSER_KD                  0
#define ARM_ACT_UPDOWN_ASSER_TIMEOUT             5000
#define ARM_ACT_UPDOWN_ASSER_POS_EPSILON         20
#define ARM_ACT_UPDOWN_ASSER_POS_LARGE_EPSILON   300 //arbitraire, a changer
#define ARM_ACT_UPDOWN_PWM_NUM                   2
#define ARM_ACT_UPDOWN_PORT_WAY                  GPIOC
#define ARM_ACT_UPDOWN_PORT_WAY_BIT              4
#define ARM_ACT_UPDOWN_MAX_PWM_WAY0              20
#define ARM_ACT_UPDOWN_MAX_PWM_WAY1              20
#define ARM_ACT_UPDOWN_MAX_VALUE				 500
#define ARM_ACT_UPDOWN_MIN_VALUE				 0

//#define ARM_ACT_RX24                             1
#define ARM_ACT_RX24                             0		//non utilisé, mais utilisé comme prefix dans arm_data (les autres moteurs c'est pareil)
#define ARM_ACT_RX24_ASSER_TIMEOUT               20
#define ARM_ACT_RX24_ASSER_POS_EPSILON           5
#define ARM_ACT_RX24_ASSER_POS_LARGE_EPSILON     10 //arbitraire, a changer
#define ARM_ACT_RX24_MAX_TORQUE_PERCENT          30
#define ARM_ACT_RX24_MAX_VALUE					 190
#define ARM_ACT_RX24_MIN_VALUE					 108

#define ARM_ACT_AX12_MID                         2
#define ARM_ACT_AX12_MID_ASSER_TIMEOUT           20
#define ARM_ACT_AX12_MID_ASSER_POS_EPSILON       5
#define ARM_ACT_AX12_MID_ASSER_POS_LARGE_EPSILON 10 //arbitraire, a changer
#define ARM_ACT_AX12_MID_MAX_TORQUE_PERCENT      30
#define ARM_ACT_AX12_MID_MAX_VALUE				 290
#define ARM_ACT_AX12_MID_MIN_VALUE				 37

#define ARM_ACT_AX12_TRIANGLE                             3
#define ARM_ACT_AX12_TRIANGLE_ASSER_TIMEOUT               20
#define ARM_ACT_AX12_TRIANGLE_ASSER_POS_EPSILON           5
#define ARM_ACT_AX12_TRIANGLE_ASSER_POS_LARGE_EPSILON     10 //arbitraire, a changer
#define ARM_ACT_AX12_TRIANGLE_MAX_TORQUE_PERCENT          30
#define ARM_ACT_AX12_TRIANGLE_MAX_VALUE					 300
#define ARM_ACT_AX12_TRIANGLE_MIN_VALUE					 0

#endif	/* ARM_CONFIG_H */

