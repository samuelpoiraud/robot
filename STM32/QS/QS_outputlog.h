/*
 *  Club Robot ESEO 2012 - 2014
 *
 *  $Id$
 *
 *  Package : Qualit� Soft
 *  Description : Propose une fonction pour envoyer des logs suivant leur importance.
 *  Auteur : Alexis
 */

#ifndef QS_OUTPUTLOG_H
#define QS_OUTPUTLOG_H

#include "QS_all.h"
#include <stdarg.h>
#include <stdio.h>

/* Macro de d'affichage d'une expression rapide */
#define display(x) debug_printf("%s : %ld\n", #x, (Sint32)(x))
#define display_float(x) {unsigned char var_ftoa[30];  debug_printf("%s : %s\n", #x, ftoa(x, var_ftoa, 3, 'f'));}


// prec indique la pr�cison (nb de chiffrers apr�s la virgule) 0 pour avoir le maximum
// si format ='s' -> affichage scientifique 1.6666666
// si format ='f' -> affichage classique 1666.6666

unsigned char *ftoa (float x, unsigned char *str, char prec, char format);

/*
 * Param�tres (optionnels, � d�finir avant l'inclusion de ce header):
 * - LOG_PREFIX: � d�finir � une chaine de caract�re. Tous les printf auront ce texte devant chaque nouvelles lignes.
 * - LOG_COMPONENT: � d�finir � LOG_PRINT_Off ou LOG_PRINT_On pour activer ou non le log. NE PAS UTILISER 0 ou 1 !! 0 == activ�, 1 == pas de log !
 */

#ifndef LOG_PREFIX
#define LOG_PREFIX ""
#endif

// La constante OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL est utilis� pour d�finir le niveau d'affichage.
//Par ex: #define OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Warning indique de n'afficher les logs de gravit� sup�rieure ou �gale � LOG_LEVEL_Warning.
//Le niveau maximum affich� par d�faut est LOG_LEVEL_Warning. OUTPUTLOG_printf n'affichera donc les messages de niveau LOG_LEVEL_Warning, LOG_LEVEL_Error ou LOG_LEVEL_Fatal seulement.

#define OUTPUTLOG_STRINGIFY(x) #x
#define OUTPUTLOG_TOSTRING(x) OUTPUTLOG_STRINGIFY(x)

typedef enum {
	LOG_LEVEL_Always,   //Affiche le message dans tous les cas
	LOG_LEVEL_Fatal,	//Le fatal devrait utilis� pour des asserts, le programme ne peut plus continuer suite � une erreur.
	LOG_LEVEL_Error,	//Une erreur est survenue, par exemple un actionneur n'a pas pu aller � la position indiqu�e. Le programme peut quand m�me continuer, mais devrait prendre conscience au moins de cette erreur (et stopper le moteur par exemple, pour ne pas le cramer ou tenter de le r�utiliser pour une autre action)
	LOG_LEVEL_Warning,	//Quelque chose s'est pass�, et pourrait causer des disfonctionnements par la suite. Par ex on d�tecte un cas dangereux qui pourrait �tre la cause de bugs mais qui ne devrait pas poser de probl�me normalement si le code �tait parfait.
	LOG_LEVEL_Info,		//Info suppl�mentaire, comme la raison d'un reset ou le nom de la carte lanc�e ...
	LOG_LEVEL_Debug,	//Affichage d'info de debuggage qui ne flooderont pas l'uart
	LOG_LEVEL_Trace		//Affichage d'info concernant le flot d'ex�cution du programme, quelles fonctions sont appell�e par ex. Lorsque du texte doit �tre affich� en masse, utilisez ce niveau la plutot que debug.
} log_level_e;

#define LOG_PRINT_Off 1
#define LOG_PRINT_On  0  //Si la macro LOG_COMPONENT n'est pas d�finie, elle vaudra 0, dans ce cas on veux afficher le texte par d�faut.

typedef int (*OUTPUTLOG_Callback)(log_level_e level, const char* formated_message);
typedef int (*OUTPUTLOG_CallbackV)(log_level_e level, const char* format, va_list vargs);

//__attribute__((format (printf, 2, 3)))  permet au compilateur de verifier l'argument format avec les suivants comme avec printf, et afficher des warning si les types ne correspondent pas (genre un %s avec un int)
void OUTPUTLOG_printf(log_level_e level, const char * format, ...) __attribute__((format (printf, 2, 3)));
void OUTPUTLOG_set_level(log_level_e level);
log_level_e OUTPUTLOG_get_level();

//Permet de rediriger la sortie de OUTPUTLOG_printf vers une fonction (l'une ou l'autre des m�thodes, pas les 2, le dernier set �crase les autres)
//OUTPUTLOG_set_callback_vargs: les arguments sont pass�s tel quel (pas de limite de taille de texte)
void OUTPUTLOG_set_callback_vargs(OUTPUTLOG_CallbackV callback);
//OUTPUTLOG_set_callback: le resultat du formatage est donn� � la fonction callback
void OUTPUTLOG_set_callback(OUTPUTLOG_Callback callback);


/* Fonction d'aide � l'utilisation */

#if LOG_COMPONENT == LOG_PRINT_Off && !defined(OUTPUTLOG_PRINT_ALL_COMPONENTS)

#define component_printf(log_level, format, ...) (void)0

#define trace_printf(...) (void)0
#define vtrace_printf(...) (void)0
#define debug_printf(...) (void)0
#define info_printf(...) (void)0
#define warn_printf(...) (void)0
#define error_printf(...) (void)0
#define fatal_printf(...) (void)0

#else

#define component_printf(log_level, format, ...) OUTPUTLOG_printf(log_level, LOG_PREFIX format, ## __VA_ARGS__)

#define trace_printf(format, ...) OUTPUTLOG_printf(LOG_LEVEL_Trace, "Trace: %s line: " OUTPUTLOG_TOSTRING(__LINE__) " " format, __func__, ## __VA_ARGS__)
#define vtrace_printf(format, ...) OUTPUTLOG_printf(LOG_LEVEL_Trace, "Trace: %s line: " OUTPUTLOG_TOSTRING(__LINE__) " in " __FILE__ ": " format, __func__, ## __VA_ARGS__)
#define debug_printf(...) OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX __VA_ARGS__)
#define info_printf(...) OUTPUTLOG_printf(LOG_LEVEL_Info, LOG_PREFIX __VA_ARGS__)
#define warn_printf(...) OUTPUTLOG_printf(LOG_LEVEL_Warning, LOG_PREFIX __VA_ARGS__)
#define error_printf(...) OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX __VA_ARGS__)
#define fatal_printf(...) OUTPUTLOG_printf(LOG_LEVEL_Fatal, LOG_PREFIX __VA_ARGS__)

#endif

// Affiche le chaine quoi qu'il arrive m�me si le log du fichier est d�sactiv� (� n'utiliser que pour du d�bug SEULEMENT !)
#define force_printf(...) OUTPUTLOG_printf(LOG_LEVEL_Always, __VA_ARGS__)

#endif // QS_OUTPUTLOG_H
