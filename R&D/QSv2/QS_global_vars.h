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
	#include "../project/Global_config.h"
	#include "../project/Global_vars_types.h"


	typedef struct
	{
		#ifdef USE_UART1RXINTERRUPT
			volatile bool_e u1rx;	// message reçu sur uart1
		#endif /* def USE_UART1RXINTERRUPT */
		#ifdef USE_UART2RXINTERRUPT
			volatile bool_e u2rx;	// message reçu sur uart2
		#endif /* def USE_UART2RXINTERRUPT */		
		/* INSERTION DES FLAGS UTILISATEUR */
		#include "../project/Global_flags.h"

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
		
		/*variables globales pour le pilote de servos */
		#ifdef USE_SERVO
			volatile Sint16 SERVO_cmd[10];
		#endif /* def USE_SERVO */
		
		/* les drapeaux */
		volatile flag_list_t flags;

		/* INSERTION DES VARIABLES GLOBALES UTILISATEUR */
		#include "../project/Global_vars.h"
	} global_data_storage_t;

	#include "../QS/QS_all.h"
	#ifndef QS_GLOBAL_VARS_C
		extern global_data_storage_t global;
	#endif /* ndef QS_GLOBAL_VARS_C */
#endif /* ndef QS_GLOBAL_VARS_H */
