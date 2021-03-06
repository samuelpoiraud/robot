/*
 *  Club Robot ESEO 2012 - 2014
 *
 *  $Id: QS_outputlog.c 923 2013-10-16 17:22:09Z amurzeau $
 *
 *  Package : Qualit� Soft
 *  Description : Propose une fonction pour envoyer des logs suivant leur importance.
 *  Auteur : Alexis
 */

#if 1
#include "QS_outputlog.h"

#include <stdio.h>
#include <stdarg.h>

#ifndef OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL
#define OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Debug
#endif

static log_level_e current_max_log_level = OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL;

void OUTPUTLOG_printf(log_level_e level, const char * format, ...) {
#if defined(OUTPUT_LOG) || defined(VERBOSE_MODE)
	va_list args_list;

	//level trop haut ou affichage desactiv�, on n'affiche pas
	if(level != LOG_LEVEL_Always && level > current_max_log_level)
		return;

	va_start(args_list, format);
	vprintf(format, args_list);
	va_end(args_list);
#else
	//Anti warning arguments non utilis�s
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
