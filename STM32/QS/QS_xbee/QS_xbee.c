/*
 *	Club Robot ESEO 2011-2012
 *	Shark & Fish
 *
 *	Fichier : QS_can_over_xbee.c
 *	Package : Qualité Soft
 *	Description : fonctions d'encapsulation des messages CAN
					pour envoi via module XBEE configuré en mode API
 *	Auteur : Nirgal
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20120224
 */


#include "QS_xbee.h"
#ifdef USE_XBEE
	#include "../QS_uart.h"
	#include "../QS_outputlog.h"
	#include "network/networkSystem.h"

	static volatile bool_e initialized = FALSE;

	void XBEE_init(void){
		initialized = TRUE;
		NETWORK_SYSTEM_init();
	}

	void XBEE_processMain(void)
	{
		if(initialized == FALSE){
			error_printf("QS_XBEE : Module non initialisé \n");
			return;
		}

		NETWORK_SYSTEM_processMain();
	}






#endif //def USE_XBEE


