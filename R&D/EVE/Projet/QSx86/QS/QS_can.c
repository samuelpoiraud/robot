/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_can.c
 *	Package : QSx86
 *	Description : Fonctions CAN
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20111206
 */

#include "QS_can.h"

#include "../QS/QS_CANmsgList.h"

#define MASK_CAN_S			0x000 /* 0b00000000000 */
/* on filtre les messages nous meme en aval */

#if defined I_AM_CARTE_ASSER	//if (I_AM == CARTE_ASSER)
	#define MY_FILTER	ASSER_FILTER
#elif defined I_AM_CARTE_ACT	//(I_AM == CARTE_ACT)
	#define MY_FILTER	ACT_FILTER
#elif defined I_AM_CARTE_STRAT	//(I_AM == CARTE_STRAT)
	#define MY_FILTER	STRAT_FILTER
#elif defined I_AM_CARTE_SUPER	//(I_AM == CARTE_SUPER)
	#define MY_FILTER	SUPER_FILTER
#elif defined I_AM_CARTE_BALISE	//(I_AM == CARTE_BALISE)
	#define MY_FILTER	BALISE_FILTER
#else
	#warning "carte inconnue : définissez I_AM_CARTE_XXXXX : voir QS_can.c"
#endif

#ifndef QS_CAN_RX_IT_PRI
	#define QS_CAN_RX_IT_PRI CAN_INT_PRI_7
#else
	#warning "QS_CAN_RX_IT_PRI redefined"
#endif /* ndef QS_CAN_RX_IT_PRI */

/* Variables globales pour le CAN */
#ifdef USE_CAN
	static direct_treatment_function_pt m_can_direct_treatment_function = NULL;
#endif /* def USE_CAN */
#ifdef USE_CAN2
	static direct_treatment_function_pt m_can2_direct_treatment_function_pt = NULL;
#endif /* def USE_CAN2 */

#if defined (USE_CAN) || defined (USE_CAN2)

	void CAN_init(void)
	{
		static bool_e initialized = FALSE;
		if(initialized)
			return;
		initialized = TRUE;

		// L'initialisation se situe actuellement dans EVE_manager_card_init()
		// A voir plus tard si des choses sont à ajouter ici...
	}

	/* Fonction de filtrage */
	static bool_e CAN_msg_filter(CAN_msg_t can_msg)
	{
		// On vérifie si le sid correspond à la carte en filtrant les sid des cartes
		if((can_msg.sid & MASK_BITS) == MY_FILTER)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	
#ifdef USE_CAN

	bool_e CAN_data_ready()
	{
		if(EVE_check_messages_numbers(eve_global.bal_id_can_to_card)>0)
		{
			return TRUE;
		}
		// Sinon, on a une valeur de 0 ou de -1 qui indique une erreur
		return FALSE;
	}	

	void CAN_send(CAN_msg_t* can_msg)
	{
		EVE_CAN_msg_t eve_can_msg;
		Uint8 i;

		eve_can_msg.mtype = QUEUE_CAN_MTYPE;
		eve_can_msg.start_msg = CAN_MESSAGE_START;
		eve_can_msg.sid = can_msg->sid;
		eve_can_msg.size = can_msg->size;
		eve_can_msg.end_msg = CAN_MESSAGE_STOP;

		for(i=0;i<CAN_MSG_DATA_SIZE;i++)
		{
			eve_can_msg.data[i] = can_msg->data[i];
		}

		if(EVE_write_new_msg_CAN(eve_global.bal_id_card_to_can,eve_can_msg)!=END_SUCCESS)
		{
			// Ajouter un truc...
		}
	}
	
	void CAN_receive(CAN_msg_t* can_msg)
	{
		EVE_CAN_msg_t eve_can_msg;
		Uint8 i;

		if(EVE_read_new_msg_CAN(eve_global.bal_id_can_to_card,&eve_can_msg)!=END_SUCCESS)
		{
			// Ajouter des messages d'erreurs...
		}
		else
		{
			can_msg->sid = eve_can_msg.sid;
			can_msg->size = eve_can_msg.size;
			for(i=0;i<CAN_MSG_DATA_SIZE;i++)
			{
				can_msg->data[i] = eve_can_msg.data[i];
			}
		}
	}

	CAN_msg_t CAN_get_next_msg()
	{
		CAN_msg_t msg_return;

		while(EVE_check_messages_numbers(eve_global.bal_id_can_to_card)>0)
		{
			// Message présent, on le récupère et on le retourne
			CAN_receive(&msg_return);
			// Filtrage des messages désirés
			if(CAN_msg_filter(msg_return)==TRUE)
			{
				// SID correct, on envoie
				return msg_return;
			}
		}

		// Pas de message présent, on retourne un message vide
		return (CAN_msg_t) {0, "\0\0\0\0\0\0\0", 0};	
	}
	
	/* Permet le traitement direct des messages CAN */
	// Pas encore implémenté
	void CAN_set_direct_treatment_function(direct_treatment_function_pt fct)
	{
		m_can_direct_treatment_function = fct;
	}

#endif /* def USE_CAN */

#ifdef USE_CAN2
	//TODO : recopier CAN1

	bool_e CAN2_data_ready()
	{
		if(EVE_check_messages_numbers(eve_global.bal_id_can_to_card)>0)
		{
			return TRUE;
		}
		// Sinon, on a une valeur de 0 ou de -1 qui indique une erreur
		return FALSE;
	}	

	void CAN2_send(CAN_msg_t* can_msg)
	{
		EVE_CAN_msg_t eve_can_msg;
		Uint8 i;

		eve_can_msg.mtype = QUEUE_CAN_MTYPE;
		eve_can_msg.start_msg = CAN_MESSAGE_START;
		eve_can_msg.sid = can_msg->sid;
		eve_can_msg.size = can_msg->size;
		eve_can_msg.end_msg = CAN_MESSAGE_STOP;

		for(i=0;i<CAN_MSG_DATA_SIZE;i++)
		{
			eve_can_msg.data[i] = can_msg->data[i];
		}

		if(EVE_write_new_msg_CAN(eve_global.bal_id_card_to_can2,eve_can_msg)!=END_SUCCESS)
		{
			// Ajouter un truc...
		}
	}


	void CAN2_receive(CAN_msg_t* can_msg)
	{
		EVE_CAN_msg_t eve_can_msg;
		Uint8 i;

		if(EVE_read_new_msg_CAN(eve_global.bal_id_can2_to_card,&eve_can_msg)!=END_SUCCESS)
		{
			// Ajouter des messages d'erreurs...
		}
		else
		{
			can_msg->sid = eve_can_msg.sid;
			can_msg->size = eve_can_msg.size;
			for(i=0;i<CAN_MSG_DATA_SIZE;i++)
			{
				can_msg->data[i] = eve_can_msg.data[i];
			}
		}
	}

	CAN_msg_t CAN2_get_next_msg()
	{
		CAN_msg_t msg_return;

		while(EVE_check_messages_numbers(eve_global.bal_id_can2_to_card)>0)
		{
			// Message présent, on le récupère et on le retourne
			CAN_receive(&msg_return);
			// Filtrage des messages désirés
			if(CAN_msg_filter(msg_return)==TRUE)
			{
				// SID correct, on envoie
				return msg_return;
			}
		}

		// Pas de message présent, on retourne un message vide
		return (CAN_msg_t) {0, "\0\0\0\0\0\0\0", 0};		
	}
	
	/* Permet le traitement direct des messages CAN */
	// Pas encore implémenté
	void CAN2_set_direct_treatment_function(direct_treatment_function_pt fct)
	{
		m_can2_direct_treatment_function = fct;
	}

#endif /* def USE_CAN2 */
#endif /* defined (USE_CAN) || defined (USE_CAN2) */
