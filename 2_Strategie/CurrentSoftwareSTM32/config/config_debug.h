/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Strategie
 *  Description : Configuration des aides au débuggage
 *  Auteur : Jacen, Alexis
 */

#ifndef CONFIG_DEBUG_H
#define CONFIG_DEBUG_H

#define OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Debug    //Pour connaitre les valeurs possibles, voir output_log.h (enum log_level_e)
//	#define OUTPUTLOG_PRINT_ALL_COMPONENTS  //Si défini, affiche les messages de tous les composants (OUTPUT_LOG_COMPONENT_* ne sont pas pris en compte, les niveau de débuggage le sont par contre)
#define OUTPUT_LOG_COMPONENT_ACTFUNCTION         LOG_PRINT_On  //LOG_PRINT_On: activé, LOG_PRINT_Off: désactivé
#define OUTPUT_LOG_COMPONENT_QUEUE               LOG_PRINT_Off
#define OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES LOG_PRINT_On   //Changement d'état de la strat


//active les debug_printf pour les sections concernées
//	#define DEBUG_ASSER_FUN
//	#define DEBUG_ACT_FUN
//	#define DEBUG_SICK
//	#define DEBUG_ELEMENTS
//	#define DEBUG_BARCODE
//	#define DEBUG_DETECTION
//	#define DEBUG_AVOIDANCE
//	#define DEBUG_POLYGON
//	#define DEBUG_TELEMETER

//envoi des messages de debug de timeout et activité des piles
//	#define USE_SEND_CAN_DEBUG

//envoie la position de l'adversaire sur le bus à chaque update (max 1 fois par seconde (a l'ecriture de ce commentaire))
//	#define USE_FOE_POS_CAN_DEBUG

//envoie de l'ajout, la mise a jour ou la suppresion d'un element de jeu sur le CAN
//	#define USE_ELEMENT_CAN_DEBUG

/*
CAN_send_debug("");
0 timeout asser goto
1 timeout asser goangle
2 timeout asser rush
3 timeout asser relative go angle
A pull
B flush
C asser far from destination

[a-z] temporaires
*/


#endif /* CONFIG_DEBUG_H */
