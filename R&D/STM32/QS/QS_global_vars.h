/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi'Tech
 *
 *  Fichier : QS_global_vars.h
 *  Package : Qualité Soft
 *  Description : Variables globales generiques communes a tous les
 *					codes du robot.
 *  Auteur : Jacen
 *  Version 20081201
 */

#ifndef QS_GLOBAL_VARS_H
	#define QS_GLOBAL_VARS_H

	#include "../QS/QS_types.h"
	//#include "../project/Global_config.h"
	#include "../Global_vars_types.h"


	typedef struct
	{
		#ifdef USE_UART1RXINTERRUPT
			volatile bool_e u1rx;	// message reçu sur uart1
		#endif /* def USE_UART1RXINTERRUPT */
		#ifdef USE_UART2RXINTERRUPT
			volatile bool_e u2rx;	// message reçu sur uart2
		#endif /* def USE_UART2RXINTERRUPT */
		#ifdef USE_CAN
			volatile bool_e canrx;	// message reçu sur le bus can
		#endif /* def USE_CAN */
		#ifdef USE_CAN2
			volatile bool_e can2rx;	// message reçu sur le bus can2
		#endif /* def USE_CAN2 */
		
		/* INSERTION DES FLAGS UTILISATEUR */
		#include "../Global_flags.h"

	} flag_list_t;



	typedef struct
	{
		/* Variables Globales pour l'UART */
		#ifdef USE_UART1RXINTERRUPT
			Uint8 u1rxbuf[UART_RX_BUF_SIZE];
			volatile Uint32 u1rxnum;
		#endif /* def USE_UART1RXINTERRUPT */
		#ifdef USE_UART2RXINTERRUPT
			Uint8 u2rxbuf[UART_RX_BUF_SIZE];
			volatile Uint32 u2rxnum;
		#endif /* def USE_UART2RXINTERRUPT */
		
		/* Variables globales pour le CAN */
		#ifdef USE_CAN
			CAN_msg_t can_buffer[CAN_BUF_SIZE];
			volatile Uint8 can_rx_num;
		#endif /* def USE_CAN */
		#ifdef USE_CAN2
			CAN_msg_t can2_buffer[CAN_BUF_SIZE];
			volatile Uint8 can2_rx_num;
		#endif /* def USE_CAN2 */
		
		/*variables globales pour le moteur pas à pas */
		#ifdef USE_STEP_MOTOR
			volatile Sint16 STEP_MOTOR_order;
		#endif /* def USE_STEP_MOTOR */

		/*variables globales pour le pilote de servos */
		#ifdef USE_SERVO
			volatile Sint16 SERVO_cmd[10];
		#endif /* def USE_SERVO */
		
		/*variables globales pour le pilote codeur sur IT */
		#ifdef USE_QEI_ON_IT
			volatile Sint16 QEI_ON_IT_count;
			volatile Sint32 correction_erreur_bizarre;
		#endif /* def USE_QEI_ON_IT */

		
		/* les drapeaux */
		volatile flag_list_t flags;

		/* INSERTION DES VARIABLES GLOBALES UTILISATEUR */
		#include "../Global_vars.h"
	} global_data_storage_t;

	#include "../QS/QS_all.h"
	#ifndef QS_GLOBAL_VARS_C
		extern global_data_storage_t global;
	#endif /* ndef QS_GLOBAL_VARS_C */
#endif /* ndef QS_GLOBAL_VARS_H */
