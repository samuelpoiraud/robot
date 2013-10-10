/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  Fichier : config_strat.h
 *  Package : Carte Strategie
 *  Description : Configuration des strat et logs
 *  Auteur : Jacen, Alexis
 *  Version $Id$
 */

#ifndef CONFIG_STRAT_H
	#define CONFIG_STRAT_H
	
	#define VERBOSE_MODE
	
	#define OUTPUT_LOG  //utilis� dans act_function et queue
	#define OUTPUT_LOG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Info    //Pour connaitre les valeurs possibles, voir output_log.h (enum log_level_e)
//	#define OUTPUT_LOG_PRINT_ALL_COMPONENTS  //Si d�fini, affiche les messages de tous les composants (OUTPUT_LOG_COMPONENT_* ne sont pas pris en compte, les niveau de d�buggage le sont par contre)
	#define OUTPUT_LOG_COMPONENT_ACTFUNCTION         LOG_PRINT_On  //LOG_PRINT_On: activ�, LOG_PRINT_Off: d�sactiv�
	#define OUTPUT_LOG_COMPONENT_QUEUE               LOG_PRINT_Off
	#define OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES LOG_PRINT_On   //Changement d'�tat de la strat
	
	
	//active les debug_printf pour les sections concern�es
//	#define DEBUG_ASSER_FUN
//	#define DEBUG_ACT_FUN
//	#define DEBUG_SICK
//	#define DEBUG_ELEMENTS
//	#define DEBUG_BARCODE
//	#define DEBUG_DETECTION
//	#define DEBUG_AVOIDANCE
//	#define DEBUG_POLYGON
//	#define DEBUG_TELEMETER

	/*
	 *	La section config carte Strat�gie
	 */


	#define STACKS_SIZE 32 //doit �tre < � 256

	//Utilis� par act_function
	#define QUEUE_SIZE	16

	
#endif /* CONFIG_STRAT_H */