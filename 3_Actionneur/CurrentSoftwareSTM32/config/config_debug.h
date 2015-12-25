/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des aides au d�buggage
 *  Auteur : Jacen, Alexis
 */

#ifndef CONFIG_DEBUG_H
#define CONFIG_DEBUG_H

#define OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL			LOG_LEVEL_Debug		//Pour connaitre les valeurs possibles, voir output_log.h (enum log_level_e)
//#define OUTPUTLOG_PRINT_ALL_COMPONENTS							//Si d�fini, affiche les messages de tous les composants (OUTPUT_LOG_COMPONENT_* ne sont pas pris en compte, les niveau de d�buggage le sont par contre)

//Activation des logs de sections pr�cise			(LOG_PRINT_On / LOG_PRINT_Off)
#define OUTPUT_LOG_COMPONENT_CANPROCESSMSG			LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_ACTQUEUEUTILS			LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_QUEUE					LOG_PRINT_Off
#define OUTPUT_LOG_COMPONENT_SELFTEST				LOG_PRINT_On

//Black
#define OUTPUT_LOG_COMPONENT_FISH_BRUSH_BACK		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_FISH_BRUSH_FRONT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_TOP_PLATE_LEFT      	LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_TOP_PLATE_RIGHT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_BLACK_SAND_CIRCLE		LOG_PRINT_On


//Pearl
#define OUTPUT_LOG_COMPONENT_LAFT_ARM				LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_RIGHT_ARM   			LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_PEARL_SAND_CIRCLE   	LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_POMPE_BACK_LEFT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_POMPE_BACK_RIGHT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_POMPE_FRONT_LEFT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_POMPE_FRONT_RIGHT		LOG_PRINT_On

//Activation de define pour l'affichage de debug
//#define AX12_DEBUG_PACKETS

#endif /* CONFIG_DEBUG_H */
