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
		vcallback(level, format, args_list);
		va_end(args_list);
	} else if(callback) {
		static char buffer[512]; //static pour être sur d'avoir la mémoire dispo, 512 pourrait causer des débordements de pile ?

		va_start(args_list, format);
		vsnprintf(buffer, 512, format, args_list);
		va_end(args_list);

		callback(level, buffer);
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

// prec indique la précison (nb de chiffrers après la virgule) 0 pour avoir le maximum
// si format ='s' -> affichage scientifique 1.6666666
// si format ='f' -> affichage classique 1666.6666

unsigned char *ftoa (float x, unsigned char *str, char prec, char format){
	/* converts a floating point number to an ascii string */
	/* x is stored into str, which should be at least 30 chars long */
	unsigned char *adpt;
	int ie, i, k, ndig, fstyle;
	double y;
	adpt=str;

	x /= 10.;

	ndig = ( prec<=0) ? 7 : (prec > 22 ? 23 : prec+1);
	if  (format == 'f' || format == 'F')
		fstyle = 1;
	else
		fstyle = 0;
	/* print in e format unless last arg is 'f' */
	ie = 0;
	/* if x negative, write minus and reverse */
	if ( x < 0){
		*str++ = '-';
		x = -x;
	}

	/* put x in range 1 <= x < 10 */
	if (x > 0.0) while (x < 1.0){
		x *= 10.0;
		ie--;
	}
	while (x >= 10.0){
		x = x/10.0;
		ie++;
	}

	/* in f format, number of digits is related to size */
	if (fstyle)
		ndig += ie;

	/* round. x is between 1 and 10 and ndig will be printed to
	right of decimal point so rounding is ... */
	for (y = i = 1; i < ndig; i++)
		y = y/10.;
	x += y/2.;

	if (x >= 10.0)
		x = 1.0; ie++; /* repair rounding disasters */

	/* now loop.  put out a digit (obtain by multiplying by
	10, truncating, subtracting) until enough digits out */
	/* if fstyle, and leading zeros, they go out special */
	if (fstyle && ie<0){
		*str++ = '0'; *str++ = '.';
		if (ndig < 0)
			ie = ie-ndig; /* limit zeros if underflow */
		for (i = -1; i > ie; i--)
			*str++ = '0';
	}
	for (i=0; i < ndig; i++){
		k = x;
		*str++ = k + '0';
		if (i == (fstyle ? ie : 0)) /* where is decimal point */
			*str++ = '.';
		x -= (y=k);
		x *= 10.0;
	}

	/* now, in estyle,  put out exponent if not zero */
	if (!fstyle && ie != 0){
		*str++ = 'E';
		if (ie < 0){
			ie = -ie;
			*str++ = '-';
		}
		for (k=100; k > ie; k /=10);
		for (; k > 0; k /=10){
			*str++ = ie/k + '0';
			ie = ie%k;
		}
	}
	*str = '\0';
	return (adpt);
}
