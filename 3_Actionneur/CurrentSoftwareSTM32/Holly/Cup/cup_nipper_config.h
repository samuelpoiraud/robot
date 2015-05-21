/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : cup_nipper_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de la pince à gobelet
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef CUP_NIPPER_CONFIG_H
#define	CUP_NIPPER_CONFIG_H

//Config
	#define CUP_NIPPER_AX12_ASSER_TIMEOUT				1000			// Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define CUP_NIPPER_AX12_ASSER_POS_EPSILON			6				// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define CUP_NIPPER_AX12_ASSER_POS_LARGE_EPSILON		28				// Si le déplacement de l'ax12 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define CUP_NIPPER_AX12_MAX_TORQUE_PERCENT			35				// Couple maximum en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define CUP_NIPPER_AX12_MAX_TEMPERATURE             60				// Température maximale en degré

//Extremum valeur
	#define CUP_NIPPER_AX12_MIN_VALUE					0
	#define CUP_NIPPER_AX12_MAX_VALUE					1024

// Configuration de l'asservissement PID
	#define CUP_NIPPER_KP								10
	#define CUP_NIPPER_KI								10
	#define CUP_NIPPER_KD								10
	#define CUP_NIPPER_KP2								10
	#define CUP_NIPPER_KI2								10
	#define CUP_NIPPER_KD2								10

	#define CUP_NIPPER_PWM_NUM							1
	#define CUP_NIPPER_PORT_WAY							GPIOA
	#define CUP_NIPPER_PORT_WAY_BIT						GPIO_Pin_8
	#define CUP_NIPPER_MAX_PWM_WAY0						80
	#define CUP_NIPPER_MAX_PWM_WAY1						80
	#define CUP_NIPPER_QEI_COEF							0x0BFF			// Random à faire !

	#define CUP_NIPPER_DCM_ASSER_TIMEOUT				2000
	#define CUP_NIPPER_DCM_ASSER_POS_EPSILON			20
	#define CUP_NIPPER_DCM_ASSER_LARGE_EPSILON			CUP_NIPPER_DCM_ASSER_POS_EPSILON


// Position moteur DCM
	#define CUP_NIPPER_DCM_IDLE_POS						10
	#define CUP_NIPPER_DCM_MID_POS						10
	#define CUP_NIPPER_DCM_UP_POS						10

// Position AX12
	#define CUP_NIPPER_AX12_OPEN_POS					512
	#define CUP_NIPPER_AX12_CLOSE_POS					512
	#define CUP_NIPPER_AX12_LOCK_POS					512

// Position initiale
	#define CUP_NIPPER_DCM_INIT_POS						CUP_NIPPER_DCM_IDLE_POS
	#define CUP_NIPPER_AX12_INIT_POS					CUP_NIPPER_AX12_OPEN_POS

	#define CUP_NIPPER_DCM_SPEED						20

#endif	/* CUP_NIPPER_CONFIG_H */

