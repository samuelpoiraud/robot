/*
 *  Club Robot ESEO 2010 - 2011
 *  Chomp
 *
 *  Fichier : QS_watchdog.h
 *  Package : Qualité Soft
 *  Description : Test des cartes génériques 2010-2011
 *  Auteur : Patrick, Erwan
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20110215
 */
 
#include "QS/QS_all.h"

#ifndef TEST_CARTE_FACTO_H
	#define TEST_CARTE_FACTO_H
	
	#include "QS/QS_uart.h"
	#include "QS/QS_watchdog.h"
	#include "QS/QS_pwm.h"
	#include "QS/QS_adc.h"
	#include "QS/QS_can.h"
	
	#define LED_ON	1
	#define LED_OFF	0
	
	#define BOUTON_ON 1
	#define BOUTON_OFF 0
	
	#define INC_PWM_PERIOD 10 //temps au bout duquel le pourcentage de la PWM incremente de 1 (en ms)

	//Mesure du coef: Volt = Coef * Entrée_analogique
	#define COEFF 0.004887
	
	void test_carte_facto_init();
	void test_carte_facto_update();
	
#endif /* ndef TEST_CARTE_FACTO_H */
