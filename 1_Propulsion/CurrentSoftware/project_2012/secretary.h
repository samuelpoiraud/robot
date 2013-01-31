 /*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : secretary.h
 *  Package : Propulsion
 *  Description : Assure la communication entre le monde ext�rieur et le syst�me.
 *					Ins�re des ordres sur la roadmap... Rend compte des actions effectu�s et des erreurs.
 *  Auteur : Asser 2006 - modif by Val' / refactoring 2009 par Nirgal (conformit� QS par Jacen)
 *  Version 201203
 */

#ifndef _SECRETARY_H_
	#define _SECRETARY_H_
	#include "QS/QS_all.h"	

		void SECRETARY_init(void);

		void SECRETARY_process_main(void);
	/*
	Fonction qui traite les messages re�us.
	Selon le cas, elle peut ajouter des ordres dans la roadmap, configurer le syst�me...
	*/
	void SECRETARY_process_CANmsg(CAN_msg_t* msg);
	
	/*
	Fonction permettant d'envoyer un message au monde ext�rieur. 
	Selon le message indiqu� en param�tre, un certain nombre 
	d'arguments seront ajout�s et envoy�s, conform�ment � la QS.
	*/
	void SECRETARY_process_send(Uint11 sid, Uint8 reason, Uint8 error_byte);
	
		void SECRETARY_process_it(void);
		
		void SECRETARY_mailbox_add(CAN_msg_t * msg);

#endif	//def _SECRETARY_H
