/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id: config_debug.h 2707 2014-10-04 13:37:29Z aguilmet $
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des aides au débuggage
 *  Auteur : Jacen, Alexis
 */

#ifndef CONFIG_DEBUG_H
#define CONFIG_DEBUG_H

/* Message debug sur l'uart 1 */
	#define VERBOSE_MODE
//#define AX12_DEBUG_PACKETS
//	#define OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Trace    //Pour connaitre les valeurs possibles, voir output_log.h (enum log_level_e)
	#define OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Debug    //Pour connaitre les valeurs possibles, voir output_log.h (enum log_level_e)

	//Décommenter pour afficher tout quelque soit les OUTPUT_LOG_COMPONENT_*
//	#define OUTPUTLOG_PRINT_ALL_COMPONENTS

	//Composants (chosir entre LOG_PRINT_On et LOG_PRINT_Off):
	//LOG_PRINT_On active l'affiche par OUTPUT_printf du composant, LOG_PRINT_Off le desactive (permet de cibler facilement certains message sans tout mettre en commentaire)
	#define OUTPUT_LOG_COMPONENT_CANPROCESSMSG LOG_PRINT_On
	#define OUTPUT_LOG_COMPONENT_ACTQUEUEUTILS LOG_PRINT_On
	#define OUTPUT_LOG_COMPONENT_QUEUE         LOG_PRINT_Off

	//Holly
	#define OUTPUT_LOG_COMPONENT_FRUIT         LOG_PRINT_On
	#define OUTPUT_LOG_COMPONENT_LANCELAUNCHER	LOG_PRINT_On

	//Wood

#endif /* CONFIG_DEBUG_H */
