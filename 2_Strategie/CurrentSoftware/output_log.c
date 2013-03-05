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

#include "output_log.h"

#ifdef OUTPUT_LOG

#include <stdio.h>
#include <stdarg.h>

#ifndef ACT_DEBUG_DEFAULT_MAX_LOG_LEVEL
#define ACT_DEBUG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Warning
#endif

static log_level_e current_max_log_level = ACT_DEBUG_DEFAULT_MAX_LOG_LEVEL;

void OUTPUTLOG_printf(log_level_e level, const char * format, ...) {
	va_list args_list;

	//level trop haut, on n'affiche pas
	if(level > current_max_log_level)
		return;

	va_start(args_list, format);
	vprintf(format, args_list);
	va_end(args_list);
}

void OUTPUTLOG_set_level(log_level_e level) {
	current_max_log_level = level;
}

log_level_e OUTPUTLOG_get_level() {
	return current_max_log_level;
}

#endif  /* OUTPUT_LOG */
