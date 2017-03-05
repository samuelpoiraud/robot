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
#define OUTPUT_LOG_COMPONENT_MOSEFT					LOG_PRINT_On

// Harry
#define OUTPUT_LOG_COMPONENT_ORE_GUN					LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_BIG_BALL_FRONT_LEFT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_BIG_BALL_FRONT_RIGHT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_BIG_BALL_BACK_LEFT			LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_BIG_BALL_BACK_RIGHT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CYLINDER_SLOPE_LEFT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CYLINDER_SLOPE_RIGHT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CYLINDER_BALANCER_LEFT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CYLINDER_BALANCER_RIGHT	LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CYLINDER_PUSHER_LEFT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CYLINDER_PUSHER_RIGHT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CYLINDER_ELEVATOR_LEFT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CYLINDER_ELEVATOR_RIGHT	LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CYLINDER_SLIDER_LEFT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CYLINDER_SLIDER_RIGHT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CYLINDER_ARM_LEFT			LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CYLINDER_ARM_RIGHT			LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_ORE_WALL					LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_ORE_ROLLER					LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_ROCKET                 	LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CYLINDER_COLOR_LEFT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CYLINDER_COLOR_RIGHT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CYLINDER_DISPOSE_LEFT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_CYLINDER_DISPOSE_RIGHT		LOG_PRINT_On

// Anne
#define OUTPUT_LOG_COMPONENT_SMALL_BALL_BACK_LEFT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_SMALL_BALL_BACK_RIGHT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_SMALL_BALL_FRONT_LEFT		LOG_PRINT_On
#define OUTPUT_LOG_COMPONENT_SMALL_BALL_FRONT_RIGHT		LOG_PRINT_On


//Activation de define pour l'affichage de debug
//#define AX12_DEBUG_PACKETS

#endif /* CONFIG_DEBUG_H */
