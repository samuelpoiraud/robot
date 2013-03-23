/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : output_log.h
 *	Package : Carte Principale
 *	Description : Propose une fonction pour envoyer des logs suivant leur importance.
 *	Auteur : Alexis
 *	Version 20130223
 */

#ifndef OUTPUTLOG_PRINTF_H
#define	OUTPUTLOG_PRINTF_H

#include "QS/QS_all.h"

// La constante OUTPUT_LOG_DEFAULT_MAX_LOG_LEVEL est utilisé pour définir le niveau d'affichage.
//Par ex: #define OUTPUT_LOG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Warning indique de n'afficher les logs de gravité supérieure ou égale à LOG_LEVEL_Warning.
//Le niveau maximum affiché par défaut est LOG_LEVEL_Warning. OUTPUTLOG_printf n'affichera donc les messages de niveau LOG_LEVEL_Warning, LOG_LEVEL_Error ou LOG_LEVEL_Fatal seulement.

// La constante OUTPUT_LOG_PRINT_ALL permet d'afficher ou non les infos quelque soit printthis.
//Utilisable pour afficher tous les textes de tous les composants.

typedef enum {
	LOG_LEVEL_Fatal,	//Le fatal devrait utilisé pour des asserts, le programme ne peut plus continuer suite à une erreur.
	LOG_LEVEL_Error,	//Une erreur est survenue, par exemple un actionneur n'a pas pu aller à la position indiquée. Le programme peut quand même continuer, mais devrait prendre conscience au moins de cette erreur (et stopper le moteur par exemple, pour ne pas le cramer ou tenter de le réutiliser pour une autre action)
	LOG_LEVEL_Warning,	//Quelque chose s'est passé, et pourrait causer des disfonctionnements par la suite. Par ex on détecte un cas dangereux qui pourrait être la cause de bugs mais qui ne devrait pas poser de problème normalement si le code était parfait.
	LOG_LEVEL_Info,		//Info supplémentaire, comme la raison d'un reset ou le nom de la carte lancée ...
	LOG_LEVEL_Debug,	//Affichage d'info de debuggage qui ne flooderont pas l'uart
	LOG_LEVEL_Trace		//Affichage d'info concernant le flot d'exécution du programme, quelles fonctions sont appellée par ex. Lorsque du texte doit être affiché en masse, utilisez ce niveau la plutot que debug.
} log_level_e;

typedef enum {
	LOG_PRINT_Off = 0,
	LOG_PRINT_On = 1
} log_print_e;

#ifdef OUTPUT_LOG
	//__attribute__((format (printf, 3, 4)))  permet au compilateur de verifier l'argument format avec les suivants comme avec printf, et afficher des warning si les types ne correspondent pas (genre un %s avec un int)
	void OUTPUTLOG_printf(log_print_e printthis, log_level_e level, const char * format, ...) __attribute__((format (printf, 3, 4)));
	void OUTPUTLOG_set_level(log_level_e level);
	log_level_e OUTPUTLOG_get_level();
	#define OUTPUTLOG_STRINGIFY(x) #x
	#define OUTPUTLOG_TOSTRING(x) OUTPUTLOG_STRINGIFY(x)
	#define OUTPUTLOG_trace(printthis) OUTPUTLOG_printf(printthis, LOG_LEVEL_Trace, "Trace: " __func__  " line: " OUTPUTLOG_TOSTRING(__LINE__))
	#define OUTPUTLOG_trace_with_filename(printthis) OUTPUTLOG_printf(printthis, LOG_LEVEL_Trace, "Trace: " __func__  " line: " OUTPUTLOG_TOSTRING(__LINE__) " in " __FILE__)
#else
	#define OUTPUTLOG_printf(...) (void)0
	#define OUTPUTLOG_set_level(...) (void)0
	#define OUTPUTLOG_get_level() LOG_LEVEL_Warning   //Niveau par defaut
	#define OUTPUTLOG_trace(...) (void)0
	#define OUTPUTLOG_trace_with_filename(...) (void)0
#endif  /* OUTPUT_LOG */

#endif	/* OUTPUTLOG_PRINTF_H */

