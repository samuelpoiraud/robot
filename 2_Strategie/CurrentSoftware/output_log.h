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

// la constante ACT_DEBUG_DEFAULT_MAX_LOG_LEVEL est utilis� pour d�finir le niveau d'affichage.
//Par ex: #define ACT_DEBUG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Warning indique de n'afficher les logs de gravit� sup�rieure ou �gale � LOG_LEVEL_Warning.
//Le niveau maximum affich� par d�faut est LOG_LEVEL_Warning. act_debug_printf n'affichera donc les messages de niveau LOG_LEVEL_Warning, LOG_LEVEL_Error ou LOG_LEVEL_Fatal seulement.

typedef enum {
	LOG_LEVEL_Fatal,	//Le fatal devrait utilis� pour des asserts, le programme ne peut plus continuer suite � une erreur.
	LOG_LEVEL_Error,	//Une erreur est survenue, par exemple un actionneur n'a pas pu aller � la position indiqu�e. Le programme peut quand m�me continuer, mais devrait prendre conscience au moins de cette erreur (et stopper le moteur par exemple, pour ne pas le cramer ou tenter de le r�utiliser pour une autre action)
	LOG_LEVEL_Warning,	//Quelque chose s'est pass�, et pourrait causer des disfonctionnements par la suite. Par ex on d�tecte un cas dangereux qui pourrait �tre la cause de bugs mais qui ne devrait pas poser de probl�me normalement si le code �tait parfait.
	LOG_LEVEL_Info,		//Info suppl�mentaire, comme la raison d'un reset ou le nom de la carte lanc�e ...
	LOG_LEVEL_Debug,	//Affichage d'info de debuggage qui ne flooderont pas l'uart
	LOG_LEVEL_Trace		//Affichage d'info concernant le flot d'ex�cution du programme, quelles fonctions sont appell�e par ex. Lorsque du texte doit �tre affich� en masse, utilisez ce niveau la plutot que debug.
} log_level_e;

#ifdef OUTPUT_LOG
	//__attribute__((format (printf, 2, 3)))  permet au compilateur de verifier l'argument format avec les suivants comme avec printf, et afficher des warning si les types ne correspondent pas (genre un %s avec un int)
	void OUTPUTLOG_printf(log_level_e level, const char * format, ...) __attribute__((format (printf, 2, 3)));
	void OUTPUTLOG_set_level(log_level_e level);
	log_level_e OUTPUTLOG_get_level();
#else
	#define OUTPUTLOG_printf(...) (void)0
	#define OUTPUTLOG_set_level(...) (void)0
	#define OUTPUTLOG_get_level() 0
#endif  /* OUTPUT_LOG */

#endif	/* OUTPUTLOG_PRINTF_H */

