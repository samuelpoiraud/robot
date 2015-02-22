/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des aides au débuggage
 *  Auteur : Jacen, Alexis
 */

#ifndef CONFIG_DEBUG_H
#define CONFIG_DEBUG_H

#define OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL			LOG_LEVEL_Debug		//Pour connaitre les valeurs possibles, voir output_log.h (enum log_level_e)
//#define OUTPUTLOG_PRINT_ALL_COMPONENTS							//Si défini, affiche les messages de tous les composants (OUTPUT_LOG_COMPONENT_* ne sont pas pris en compte, les niveau de débuggage le sont par contre)

//Activation des logs de sections précise			(LOG_PRINT_On / LOG_PRINT_Off)
#define OUTPUT_LOG_COMPONENT_CANPROCESSMSG			LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_ACTQUEUEUTILS			LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_QUEUE					LOG_PRINT_Off
#define OUTPUT_LOG_COMPONENT_SELFTEST				LOG_PRINT_On

//Holly
#define OUTPUT_LOG_COMPONENT_POP_COLLECT_LEFT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_POP_COLLECT_RIGHT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_POP_DROP_LEFT			LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_POP_DROP_RIGHT			LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_BACK_SPOT_LEFT			LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_BACK_SPOT_RIGHT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_SPOT_POMPE_RIGHT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_SPOT_POMPE_LEFT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CARPET_LAUNCHER_LEFT	LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CARPET_LAUNCHER_RIGHT	LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_ELEVATOR				LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_PINCEMI				LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_STOCK					LOG_PRINT_On

//Wood
#define OUTPUT_LOG_COMPONENT_CLAP					LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_PINCE_DEVANT			LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_PINCE_DROITE			LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_PINCE_GAUCHE			LOG_PRINT_On

//Activation de define pour l'affichage de debug
//#define AX12_DEBUG_PACKETS

#endif /* CONFIG_DEBUG_H */
