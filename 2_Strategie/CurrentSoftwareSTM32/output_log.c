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
#include "Supervision/SD/SD.h"

#ifdef OUTPUT_LOG

#include <stdio.h>
#include <stdarg.h>

#define OUTPUTLOG_BUFFER_SIZE 128

#ifndef OUTPUT_LOG_DEFAULT_MAX_LOG_LEVEL
#define OUTPUT_LOG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Warning
#endif

static log_level_e current_max_log_level = OUTPUT_LOG_DEFAULT_MAX_LOG_LEVEL;

void OUTPUTLOG_printf(log_print_e printthis, log_level_e level, const char * format, ...) {
	va_list args_list;

	//level trop haut ou affichage desactivé, on n'affiche pas
#ifndef OUTPUT_LOG_PRINT_ALL_COMPONENTS
	if(printthis == LOG_PRINT_Off || level > current_max_log_level)
		return;
#else
	if(level > current_max_log_level)
		return;
#endif

	char buf[OUTPUTLOG_BUFFER_SIZE];
	Uint16 ret;
	static bool_e was_newline = TRUE;
	bool_e b_insert_time = was_newline;

	va_start(args_list, format);
	ret = vsnprintf(buf, OUTPUTLOG_BUFFER_SIZE, format, args_list);
	va_end(args_list);

	//if true, vsnprintf truncated the output
	if(ret >= OUTPUTLOG_BUFFER_SIZE)
		ret = OUTPUTLOG_BUFFER_SIZE-1;

	SD_new_event(FROM_SOFT, NULL, buf, b_insert_time);
	puts(buf);

	was_newline = buf[ret-1] == '\n'; //Si la ligne à un '\n' à la fin, on ajoutera un timestamp au prochain printf
}

void OUTPUTLOG_set_level(log_level_e level) {
	current_max_log_level = level;
}

log_level_e OUTPUTLOG_get_level() {
	return current_max_log_level;
}

#endif  /* OUTPUT_LOG */
