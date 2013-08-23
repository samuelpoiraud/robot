/*
 *	Club Robot ESEO 2010 - 2011
 *	Chek'Norris
 *
 *	Fichier : Selftest.c
 *	Package : Supervision
 *	Description : Envoi et réception des messages de selftest.
 *	Auteur : Ronan & Patman
 *	Version 20110120
 */

#define SELFTEST
#define TEMPS_WATCHDOG_ACT 30000	// en ms
#define TEMPS_WATCHDOG_STRAT 1000	// en ms
#define TEMPS_WATCHDOG_ASSER 10000	// en ms
#define TEMPS_WATCHDOG_BALISE 12000	// en ms
#include "Selftest.h"	

volatile bool_e flag_act;
volatile bool_e flag_asser;
volatile bool_e flag_strat;
volatile bool_e flag_beacon;
volatile bool_e selftest_enable;

typedef struct{
	bool_e etat_moteur_gche;
	bool_e etat_moteur_dt;
	bool_e etat_roue_codeuse_gche;
	bool_e etat_roue_codeuse_dte;
	Uint8 etats_capteurs;
	bool_e biroute_placee;
	bool_e etat_balise_ir;
	bool_e etat_balise_us;
	bool_e synchro_balise;
	bool_e test_strat;
	bool_e test_asser;
	bool_e test_act;
	bool_e test_balise;
}test_carte_t;
volatile test_carte_t test_cartes;

void SELFTEST_init()
{
	WATCHDOG_init();
	
	// initialisation des résultats des tests unitaires de chaque module à ERREUR
	// (voir définition des tests dans structure test_carte_t dans le header Global_vars_types.h)
	test_cartes.etat_moteur_gche = FALSE;
	test_cartes.etat_moteur_dt = FALSE;
	test_cartes.etat_roue_codeuse_gche = FALSE;
	test_cartes.etat_roue_codeuse_dte = FALSE;
	test_cartes.etats_capteurs = FALSE;
	test_cartes.biroute_placee = FALSE;
	test_cartes.etat_balise_ir = FALSE;
	test_cartes.etat_balise_us = FALSE;
	test_cartes.synchro_balise = FALSE;
	
	selftest_enable = FALSE;
	
	// initialisation des flags correspondant aux timeout pour les tests de chaque module
	flag_act = FALSE;
	flag_strat = FALSE;
	flag_asser = FALSE;
	flag_beacon = FALSE;
	
	// initialisation des résultats des tests globales de tous les modules à FALSE
	test_cartes.test_strat = FALSE;
	test_cartes.test_asser = FALSE;
	test_cartes.test_act = FALSE;
	test_cartes.test_balise = FALSE;

	LED_SELFTEST = FALSE;

}

void SELFTEST_update(CAN_msg_t* CAN_msg_received)
{
	static enum
	{
		ATTENTE_APPUI_BOUTON,
		ENVOI_MESSAGE_ACT,
		ATTENTE_REPONSE_ACT,
		ENVOI_MESSAGE_STRAT,
		ATTENTE_REPONSE_STRAT,
		ENVOI_MESSAGE_ASSER,
		ATTENTE_REPONSE_ASSER,
		ENVOI_MESSAGE_BALISE,
		ATTENTE_REPONSE_BALISE,
		PROCEDURE_TERMINEE
	} etat = 0;
	
	Uint8 erreur_reception_ir,erreur_reception_us;
	Uint16 distance_mm;
	Uint8 etat_synchro;
	
	static Uint8 fin_test_balise = 0;
	static watchdog_id_t watchdog_selftest;
	CAN_msg_t CAN_msg_sent;
	
	switch(etat)
	{
		case ATTENTE_APPUI_BOUTON:
			if(selftest_enable && !global.env.match_started)
			{
				debug_printf("\r\n_________________________ SELFTEST __________________________\r\n\r\n");
				//LED_init(); // Changement d'utilisation voir selftest balises
				etat = ENVOI_MESSAGE_ACT;
				//etat = ENVOI_MESSAGE_STRAT;
			}	
			break;
			
		case ENVOI_MESSAGE_ACT:
			CAN_msg_sent.sid = SUPER_ASK_ACT_SELFTEST;
			CAN_msg_sent.size = 0;
			debug_printf("-> demande selftest carte actionneur\r\n");
			//TODO : CAN_send(&CAN_msg_sent);
			watchdog_selftest = WATCHDOG_create_flag(TEMPS_WATCHDOG_ACT, (bool_e*) &(flag_act));
			etat = ATTENTE_REPONSE_ACT;
			break;
			
		case ATTENTE_REPONSE_ACT:
			if(CAN_msg_received != NULL && flag_act == FALSE)
			{
				debug_printf("----- Reception resultat test carte actionneur -----\r\n"); 
				// reception du message CAN et analyse des données
				
			
				WATCHDOG_stop(watchdog_selftest);
				etat = ENVOI_MESSAGE_STRAT;	
				
				//Allumage de la LED verte actionneur si tous les tests sont bons
			}
			else if(flag_act == TRUE)
			{
				debug_printf("TIMEOUT carte actionneur\r\n");
				etat = ENVOI_MESSAGE_STRAT;
			}
			break;
			
		case ENVOI_MESSAGE_STRAT:
			debug_printf("-> demande selftest carte strategie\r\n");
			CAN_msg_sent.sid = SUPER_ASK_STRAT_SELFTEST;
			CAN_msg_sent.size = 0;
			//TODO : CAN_send(&CAN_msg_sent);
			watchdog_selftest = WATCHDOG_create_flag(TEMPS_WATCHDOG_STRAT, (bool_e*) &(flag_strat));
			etat = ATTENTE_REPONSE_STRAT;
			break;
			
		case ATTENTE_REPONSE_STRAT:
			if(CAN_msg_received != NULL && flag_strat == FALSE){
				debug_printf("----- Reception resultat test carte strategie -----\r\n");
				test_cartes.etats_capteurs = CAN_msg_received->data[0];
				
				if(test_cartes.etats_capteurs & 0b00000001)
					debug_printf("Capteur DT50 gauche OK\r\n");
				else
					debug_printf("Capteur DT50 gauche ERREUR\r\n");
					
				if(test_cartes.etats_capteurs & 0b00000010)
					debug_printf("Capteur DT50 droit OK\r\n");
				else
					debug_printf("Capteur DT50 droit ERREUR\r\n");
					
				if(test_cartes.etats_capteurs & 0b00000100)
					debug_printf("Capteur DT50 avant OK\r\n");
				else
					debug_printf("Capteur DT50 avant ERREUR\r\n");

				if(test_cartes.etats_capteurs & 0b00001000)
					debug_printf("Capteur DT10 0 OK\r\n");
				else
					debug_printf("Capteur DT10 0 ERREUR\r\n");
					
				if(test_cartes.etats_capteurs & 0b00010000)
					debug_printf("Capteur DT10 1 OK\r\n");
				else
					debug_printf("Capteur DT10 1 ERREUR\r\n");

				if(test_cartes.etats_capteurs & 0b00100000)
					debug_printf("Capteur DT10 2 OK\r\n");
				else
					debug_printf("Capteur DT10 2 ERREUR\r\n");

				if(test_cartes.etats_capteurs & 0b01000000)
					debug_printf("Capteur DT10 3 OK\r\n");
				else
					debug_printf("Capteur DT10 3 ERREUR\r\n");
					
				test_cartes.biroute_placee = CAN_msg_received->data[1];
				if(test_cartes.biroute_placee)
					debug_printf("Biroute PLACEE\r\n");
				else
					debug_printf("Biroute OUBLIEE\r\n");
				WATCHDOG_stop(watchdog_selftest);
				etat = ENVOI_MESSAGE_ASSER;
				if(test_cartes.biroute_placee == TRUE && (test_cartes.etats_capteurs & 0b01111111) == 0b01111111)
				{
					test_cartes.test_strat = TRUE;
				}
			}
			else if(flag_strat)
			{
				debug_printf("TIMEOUT carte strategie\r\n");
				etat = ENVOI_MESSAGE_ASSER;
			}	
			break;
			
		case ENVOI_MESSAGE_ASSER:
			debug_printf("-> demande selftest carte asser\r\n");
			CAN_msg_sent.sid = SUPER_ASK_ASSER_SELFTEST;
			CAN_msg_sent.data[0] = (global.env.color==BLUE)?FORWARD:REAR;
			CAN_msg_sent.size = 1;
			CAN_send(&CAN_msg_sent);
			watchdog_selftest = WATCHDOG_create_flag(TEMPS_WATCHDOG_ASSER, (bool_e*) &(flag_asser));
			etat = ATTENTE_REPONSE_ASSER;
			break;
			
		case ATTENTE_REPONSE_ASSER:
			if(CAN_msg_received != NULL && flag_asser == FALSE)
			{
				debug_printf("----- Reception resultat test carte asser -----\r\n");
				test_cartes.etat_moteur_gche = CAN_msg_received->data[0];
				test_cartes.etat_moteur_dt = CAN_msg_received->data[1];
				test_cartes.etat_roue_codeuse_gche = CAN_msg_received->data[2];
				test_cartes.etat_roue_codeuse_dte = CAN_msg_received->data[3];
				
				if(test_cartes.etat_moteur_gche)
					debug_printf("Etat moteur gauche OK\r\n");
				else
					debug_printf("Etat moteur gauche ERREUR\r\n");
					
				if(test_cartes.etat_moteur_dt)
					debug_printf("Etat moteur droit OK\r\n");
				else
					debug_printf("Etat moteur droit ERREUR\r\n");
					
				if(test_cartes.etat_roue_codeuse_gche)
					debug_printf("Etat roue codeuse gauche OK\r\n");
				else
					debug_printf("Etat roue codeuse gauche ERREUR\r\n");
					
				if(test_cartes.etat_roue_codeuse_dte)
					debug_printf("Etat roue codeuse droite OK\r\n");
				else
					debug_printf("Etat roue codeuse droite ERREUR\r\n");
					
				WATCHDOG_stop(watchdog_selftest);
				etat = ENVOI_MESSAGE_BALISE;
				
				if(test_cartes.etat_moteur_gche == TRUE && test_cartes.etat_moteur_dt == TRUE
					&& test_cartes.etat_roue_codeuse_gche == TRUE && test_cartes.etat_roue_codeuse_dte == TRUE)
				{
					test_cartes.test_asser = TRUE;
				}		
			}
			else if(flag_asser)
			{
				debug_printf("TIMEOUT carte asser\r\n");
				etat = ENVOI_MESSAGE_BALISE;
			}
			break;
			
		case ENVOI_MESSAGE_BALISE:
			CAN_msg_sent.sid = SUPER_ASK_BEACON_SELFTEST;
			CAN_msg_sent.size = 0;
			debug_printf("-> demande selftest balise\r\n");
			//TODO  : CAN_send(&CAN_msg_sent);
			watchdog_selftest = WATCHDOG_create_flag(TEMPS_WATCHDOG_BALISE, (bool_e*) &(flag_beacon));
			etat = ATTENTE_REPONSE_BALISE;
			break; 
			
		case ATTENTE_REPONSE_BALISE:
			if(CAN_msg_received != NULL && flag_beacon == FALSE)
			{
				if(CAN_msg_received->sid == BEACON_IR_SELFTEST)
				{
					debug_printf("----- Reception resultat test balise IR -----\r\n");
					fin_test_balise++;
					erreur_reception_ir = CAN_msg_received->data[0];
					if(!erreur_reception_ir)
					{
						test_cartes.etat_balise_ir = TRUE;
						debug_printf("test IR OK\r\n");
					}
					else
					{
						debug_printf("test IR ERREUR\r\n");	
					}
				}
				if(CAN_msg_received->sid == BEACON_US_SELFTEST)
				{
					debug_printf("----- Reception resultat test balise US -----\r\n");
					fin_test_balise++;
					erreur_reception_us = CAN_msg_received->data[1];
					distance_mm = 10*CAN_msg_received->data[2];
					etat_synchro = CAN_msg_received->data[3];
					if(!erreur_reception_us)
					{
						test_cartes.etat_balise_us = TRUE;
						debug_printf("test US OK\r\n");
					}
					else
					{
						debug_printf("test US ERREUR\r\n");
						debug_printf("erreur_reception_us : %d\r\n",erreur_reception_us);
					}
					if(etat_synchro > TEMPS_SYNCHRO)
					{
						test_cartes.synchro_balise = TRUE;
						debug_printf("synchro OK -> nombre de secondes : %d\r\n",etat_synchro);
					}
					else
					{
						debug_printf("synchro ERREUR -> nombre de secondes : %d\r\n",etat_synchro);
					}
					debug_printf("distance estimee : %d mm\r\n",distance_mm);
				}
			}
			if(!flag_beacon && fin_test_balise == NOMBRE_TESTS_BALISE)
			{
				fin_test_balise = 0;
				WATCHDOG_stop(watchdog_selftest);
				if(test_cartes.synchro_balise && test_cartes.etat_balise_us
					&& test_cartes.etat_balise_ir)
				{
					test_cartes.test_balise = TRUE;
				}
				etat = PROCEDURE_TERMINEE;
			}
			if(flag_beacon)
			{
				debug_printf("TIMEOUT balise\r\n");
				etat = PROCEDURE_TERMINEE;
			}	
			break;
			
		case PROCEDURE_TERMINEE:
			if(test_cartes.test_act && test_cartes.test_strat
				&& test_cartes.test_asser && test_cartes.test_balise)
			{
				LED_SELFTEST = TRUE;
			}
			debug_printf("______________________ FIN SELFTEST ________________________\r\n");
			etat = ATTENTE_APPUI_BOUTON;
			selftest_enable = FALSE;
			break;	
				
		default : 
			break;
	}
}

void get_selftest_result(Uint8 * propulsion, Uint8 * various, Uint8 * boards)
{
	*propulsion = 	(Uint8)(test_cartes.etat_moteur_gche)		<< 1 |
	(Uint8)(test_cartes.etat_moteur_dt) 			<< 2 |
	(Uint8)(test_cartes.etat_roue_codeuse_gche) 	<< 3 |
	(Uint8)(test_cartes.etat_roue_codeuse_dte)	<< 4;

	*various = 	(Uint8)(test_cartes.etats_capteurs) 				 |
	(Uint8)(test_cartes.biroute_placee)			<< 1 |
	(Uint8)(test_cartes.etat_balise_ir) 			<< 2 |
	(Uint8)(test_cartes.etat_balise_us)			<< 3 |
	(Uint8)(test_cartes.synchro_balise) 			<< 4 ;

	*boards =	(Uint8)(test_cartes.test_strat) 				 	 |
	(Uint8)(test_cartes.test_asser)				<< 1 |
	(Uint8)(test_cartes.test_act) 				<< 2 |
	(Uint8)(test_cartes.test_balise) 			<< 3;
}

//Flag qui permet de lancer la fonction SELFTEST_balise_update
volatile static bool_e balise_test_flag = FALSE;

//Fonction qui leve le flag précédent appelée par le timer4: 250ms
void beacon_flag_update(){
	balise_test_flag = TRUE;
}


void led_us_update(selftest_beacon_e state){
		switch(state)
		{
			case BEACON_ERROR:
				LED_BEACON_US_GREEN = LED_OFF;
				LED_BEACON_US_RED = !LED_BEACON_US_RED;
				break;
			case BEACON_NEAR:
				LED_BEACON_US_GREEN = !LED_BEACON_US_GREEN;
				LED_BEACON_US_RED = LED_BEACON_US_GREEN;
				break;
			case BEACON_FAR:
				LED_BEACON_US_RED = LED_OFF;
				LED_BEACON_US_GREEN = !LED_BEACON_US_GREEN;
				break;
			default:
				LED_BEACON_US_RED = LED_ON;
				LED_BEACON_US_GREEN = LED_OFF;
				break;
		}
}

void led_ir_update(selftest_beacon_e state)
	{
	 	switch(state)
	 	{
			case BEACON_ERROR:
				LED_BEACON_IR_GREEN = LED_OFF;
				LED_BEACON_IR_RED = !LED_BEACON_IR_RED;
				break;
			case BEACON_NEAR:
				LED_BEACON_IR_GREEN = !LED_BEACON_IR_GREEN;
				LED_BEACON_IR_RED = LED_BEACON_IR_GREEN;
				break;
			case BEACON_FAR:
				LED_BEACON_IR_RED = LED_OFF;
				LED_BEACON_IR_GREEN = !LED_BEACON_IR_GREEN;
				break;
			default:
				LED_BEACON_IR_RED = LED_ON;
				LED_BEACON_IR_GREEN = LED_OFF;
				break;
		}
}


void SELFTEST_balise_update(){
	static Uint8 counter = 8;
	
	if(balise_test_flag){
		if(global.env.match_started == FALSE)
		{
			counter--;
			if(counter == 0)
			{
				counter = 200;
				//TODO : CAN_send_sid(BEACON_ENABLE_PERIODIC_SENDING);
			}
		}
		balise_test_flag = FALSE;
	}
}

void BUTTON_selftest(void)
{
	selftest_enable = TRUE;
}



volatile static struct {
	Uint8 beacon_error_ir_counter[8];
	Uint8 beacon_error_us_counter[8];
	Uint16 report_counter;
}beacon_error_report = {{0},{0}, 0};

void SELFTEST_beacon_counter_init(){
	Uint8 i;
	for(i=0;i<8;i++)
		beacon_error_report.beacon_error_ir_counter[i] = 0;
	for(i=0;i<8;i++)
		beacon_error_report.beacon_error_us_counter[i] = 0;
	beacon_error_report.report_counter = 0;
}

void error_counters_update(CAN_msg_t * msg){
	switch(msg->sid){
		case BEACON_ADVERSARY_POSITION_IR:
			if(msg->data[0] & 0b0)
				beacon_error_report.beacon_error_ir_counter[msg->data[0]]++;
			if(msg->data[0] & 0b00000001)
				beacon_error_report.beacon_error_ir_counter[msg->data[1]]++;
			if(msg->data[0] & 0b00000010)
				beacon_error_report.beacon_error_ir_counter[msg->data[2]]++;
			if(msg->data[0] & 0b00000100)
				beacon_error_report.beacon_error_ir_counter[msg->data[3]]++;
			if(msg->data[0] & 0b00001000)
				beacon_error_report.beacon_error_ir_counter[msg->data[4]]++;
			if(msg->data[0] & 0b00010000)
				beacon_error_report.beacon_error_ir_counter[msg->data[5]]++;
			if(msg->data[0] & 0b10000000)
				beacon_error_report.beacon_error_ir_counter[msg->data[6]]++;

			if(msg->data[4] & 0b0)
				beacon_error_report.beacon_error_ir_counter[msg->data[0]]++;
			if(msg->data[4] & 0b00000001)
				beacon_error_report.beacon_error_ir_counter[msg->data[1]]++;
			if(msg->data[4] & 0b00000010)
				beacon_error_report.beacon_error_ir_counter[msg->data[2]]++;
			if(msg->data[4] & 0b00000100)
				beacon_error_report.beacon_error_ir_counter[msg->data[3]]++;
			if(msg->data[4] & 0b00001000)
				beacon_error_report.beacon_error_ir_counter[msg->data[4]]++;
			if(msg->data[4] & 0b00010000)
				beacon_error_report.beacon_error_ir_counter[msg->data[5]]++;
			if(msg->data[4] & 0b10000000)
				beacon_error_report.beacon_error_ir_counter[msg->data[6]]++;

			/*
			if(beacon_error_report.beacon_error_ir_counter[msg->data[0]] < 0XFF)
				beacon_error_report.beacon_error_ir_counter[msg->data[0]]++;
			if(beacon_error_report.beacon_error_ir_counter[msg->data[4]] < 0XFF)
				beacon_error_report.beacon_error_ir_counter[msg->data[4]]++;
			 */
			beacon_error_report.report_counter++;
			break;
		case BEACON_ADVERSARY_POSITION_US:
			if(msg->data[0] & 0b0)
				beacon_error_report.beacon_error_us_counter[msg->data[0]]++;
			if(msg->data[0] & 0b00000001)
				beacon_error_report.beacon_error_us_counter[msg->data[1]]++;
			if(msg->data[0] & 0b00000010)
				beacon_error_report.beacon_error_us_counter[msg->data[2]]++;
			if(msg->data[0] & 0b00000100)
				beacon_error_report.beacon_error_us_counter[msg->data[3]]++;
			if(msg->data[0] & 0b00001000)
				beacon_error_report.beacon_error_us_counter[msg->data[4]]++;
			if(msg->data[0] & 0b00010000)
				beacon_error_report.beacon_error_us_counter[msg->data[5]]++;
			if(msg->data[0] & 0b10000000)
				beacon_error_report.beacon_error_us_counter[msg->data[6]]++;

			if(msg->data[4] & 0b0)
				beacon_error_report.beacon_error_us_counter[msg->data[0]]++;
			if(msg->data[4] & 0b00000001)
				beacon_error_report.beacon_error_us_counter[msg->data[1]]++;
			if(msg->data[4] & 0b00000010)
				beacon_error_report.beacon_error_us_counter[msg->data[2]]++;
			if(msg->data[4] & 0b00000100)
				beacon_error_report.beacon_error_us_counter[msg->data[3]]++;
			if(msg->data[4] & 0b00001000)
				beacon_error_report.beacon_error_us_counter[msg->data[4]]++;
			if(msg->data[4] & 0b00010000)
				beacon_error_report.beacon_error_us_counter[msg->data[5]]++;
			if(msg->data[4] & 0b10000000)
				beacon_error_report.beacon_error_us_counter[msg->data[6]]++;


			/*
			if(beacon_error_report.beacon_error_us_counter[msg->data[0]] < 0xFF)
				beacon_error_report.beacon_error_us_counter[msg->data[0]]++;
			if(beacon_error_report.beacon_error_us_counter[msg->data[4]] < 0xFF)
				beacon_error_report.beacon_error_us_counter[msg->data[4]]++;*/
			beacon_error_report.report_counter++;
			break;
		default:
			break;
	}
}

void SELFTEST_get_match_report_IR(CAN_msg_t * msg){
	int i;
	msg->sid = IR_ERROR_RESULT;
	msg->size = 8;
	for(i=0;i<8;i++)
		msg->data[i] = beacon_error_report.beacon_error_ir_counter[i];
}

void SELFTEST_get_match_report_US(CAN_msg_t * msg){
	int i;
	msg->sid = US_ERROR_RESULT;
	msg->size = 8;
	for(i=0;i<8;i++)
		msg->data[i] = beacon_error_report.beacon_error_us_counter[i];
}
