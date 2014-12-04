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

#define OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Debug		//Pour connaitre les valeurs possibles, voir output_log.h (enum log_level_e)
//#define OUTPUTLOG_PRINT_ALL_COMPONENTS					//Si défini, affiche les messages de tous les composants (OUTPUT_LOG_COMPONENT_* ne sont pas pris en compte, les niveau de débuggage le sont par contre)

//Activation des logs de sections précise			(LOG_PRINT_On / LOG_PRINT_Off)
#define OUTPUT_LOG_COMPONENT_ACTFUNCTION			LOG_PRINT_Off
#define OUTPUT_LOG_COMPONENT_QUEUE					LOG_PRINT_Off
#define OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES	LOG_PRINT_On		//Changement d'état de la strat
#define OUTPUT_LOG_COMPONENT_PROP_FUNCTION			LOG_PRINT_Off
#define OUTPUT_LOG_COMPONENT_DETECTION				LOG_PRINT_Off
#define OUTPUT_LOG_COMPONENT_AVOIDANCE				LOG_PRINT_Off

//Activation de define pour l'affichage de debug
//#define DEBUG_POLYGON

#endif /* CONFIG_DEBUG_H */
