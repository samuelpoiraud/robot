/*
 *  Club Robot ESEO 2012 - 2014
 *
 *  $Id$
 *
 *  Package : Qualité Soft
 *  Description : Propose une fonction pour envoyer des logs suivant leur importance.
 *  Auteur : Alexis
 */

#include "QS_outputlog.h"
#ifdef NEW_CONFIG_ORGANISATION
	#include "config_debug.h"
#endif

#include <stdio.h>

#ifndef OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL
#define OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Debug
#warning "Attention, niveau d'affichage printf non défini, mis à LOG_LEVEL_Debug par defaut (OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL)"
#endif

static log_level_e current_max_log_level = OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL;
static OUTPUTLOG_CallbackV vcallback;
static OUTPUTLOG_Callback callback;

void OUTPUTLOG_printf(log_level_e level, const char * format, ...) {
#ifdef VERBOSE_MODE
	va_list args_list;

	//level trop haut ou affichage desactivé, on n'affiche pas
	if(level != LOG_LEVEL_Always && level > current_max_log_level)
		return;

	if(vcallback) {
		va_start(args_list, format);
		vcallback(format, args_list);
		va_end(args_list);
	} else if(callback) {
		static char buffer[512]; //static pour être sur d'avoir la mémoire dispo, 512 pourrait causer des débordements de pile ?

		va_start(args_list, format);
		vsnprintf(buffer, 512, format, args_list);
		va_end(args_list);

		callback(buffer);
	}

	va_start(args_list, format);
	vprintf(format, args_list);
	va_end(args_list);
#else
	//Anti warning arguments non utilisés
	level = level;
	format = format;
#endif
}

void OUTPUTLOG_set_level(log_level_e level) {
	current_max_log_level = level;
	if(current_max_log_level > 200)
		printf("current_max_log_level n'est jamais > 200, mais on est sur de compiler le support du printf ...");
}

log_level_e OUTPUTLOG_get_level() {
	return current_max_log_level;
}

void OUTPUTLOG_set_callback_vargs(OUTPUTLOG_CallbackV callback) {
	callback = NULL;
	vcallback = callback;
}

void OUTPUTLOG_set_callback(OUTPUTLOG_Callback callback) {
	vcallback = NULL;
	callback = callback;
}
