/*
 *  Club Robot ESEO 2012 - 2014
 *
 *  $Id$
 *
 *  Package : Qualité Soft
 *  Description : Propose une fonction pour envoyer des logs suivant leur importance.
 *  Auteur : Alexis
 */

#if 0
#include "QS_outputlog.h"

#include <stdio.h>
#include <stdarg.h>

#ifndef OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL
#define OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Warning
#endif

static log_level_e current_max_log_level = OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL;

void OUTPUTLOG_printf(log_level_e level, const char * format, ...) {
#ifdef OUTPUT_LOG
	va_list args_list;

	//level trop haut ou affichage desactivé, on n'affiche pas
	if(level > current_max_log_level)
		return;

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

#endif /* 0 */
