/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_EVE_hard_emulation.c
 *	Package : QS_cartes
 *	Description : Gestion des fonctions hardware du dsPIC qui n'existent plus sur x86
 *	Auteur : Julien Franchineau & François Even
 *	Version 20120105
 */

#define QS_EVE_HARD_EMULATION_C

#include "QS_EVE_hard_emulation.h"

static registers_save_t registers_save;

// Fonction d'initialisation des variables liées au hard du dsPIC
void EVE_HARD_init()
{
	// Initialisation des variables des fonctions d'erreurs de démarrage du dsPIC
	RCON = 0;
	_MATHERR = 0;
	_STKERR = 0;
	_ADDRERR = 0;
	_OSCFAIL = 0;

	// Flags des timers
	IFS0bits.T1IF = 0;
	IFS0bits.T2IF = 0;
	IFS0bits.T3IF = 0;
	IFS0bits.T4IF = 0;

	// Initialisation des ports LATD (ports des LEDs) 
	LATDbits.LATD8 = 0;
	LATDbits.LATD9 = 0;
	LATDbits.LATD10 = 0;
	LATDbits.LATD11 = 0;
	LATDbits.LATD12 = 0;
	LATDbits.LATD13 = 0;

	// Initialisation des ports D (ports des boutons)
	PORTDbits.RD4 = 0;
	PORTDbits.RD5 = 0;
	PORTDbits.RD6 = 0;
	PORTDbits.RD7 = 0;

	// Initialisation des ports LATE (ports des PWM)
	LATEbits.LATE0 = 0;
	LATEbits.LATE1 = 0;
	LATEbits.LATE2 = 0;
	LATEbits.LATE3 = 0;
	LATEbits.LATE4 = 0;
	LATEbits.LATE5 = 0;
	LATEbits.LATE6 = 0;
	LATEbits.LATE7 = 0;
	LATEbits.LATE8 = 0;
	LATEbits.LATE9 = 0;

	// Initialisations diverses et variées
	LATGbits.LATG6 = 0;
	LATGbits.LATG7 = 0;
	PORTGbits.RG8 = 0;



	// Initialisation du registre de sauvegarde des états
	registers_save.LATDbits.LATD8 = 0;
	registers_save.LATDbits.LATD9 = 0;
	registers_save.LATDbits.LATD10 = 0;
	registers_save.LATDbits.LATD11 = 0;
	registers_save.LATDbits.LATD12 = 0;
	registers_save.LATDbits.LATD13 = 0;

	registers_save.LATGbits.LATG6 = 0;
	registers_save.LATGbits.LATG7 = 0;

	registers_save.PORTGbits.RG8 = 0;
}

// Fonction de vérification des changements d'états des variables liées aux ports
void EVE_HARD_check_ports_modifications()
{
	// Vérification des LEDs
	EVE_HARD_check_leds();

	// Vérification des PWM
	EVE_HARD_check_pwm();
}

// Fonction de vérification des changements d'état des LEDs
void EVE_HARD_check_leds()
{
	EVE_QS_data_msg_t data_msg;
	data_msg = EVE_init_msg_qs_data();

	/* Vérification des LEDs */
	// LED_RUN
	if(registers_save.LATDbits.LATD8 != LATDbits.LATD8)
	{
		registers_save.LATDbits.LATD8 = LATDbits.LATD8;
		data_msg.data_id = QS_LED_RUN;
		data_msg.data1 = LATDbits.LATD8;
		// Envoi du message
		EVE_write_new_msg_QS(eve_global.bal_id_card_to_ihm_qs_data,data_msg);
	}
	// LED_CAN
	if(registers_save.LATDbits.LATD9 != LATDbits.LATD9)
	{
		registers_save.LATDbits.LATD9 = LATDbits.LATD9;
		data_msg.data_id = QS_LED_CAN;
		data_msg.data1 = LATDbits.LATD9;
		// Envoi du message
		EVE_write_new_msg_QS(eve_global.bal_id_card_to_ihm_qs_data,data_msg);
	}
	// LED_UART
	if(registers_save.LATDbits.LATD10 != LATDbits.LATD10)
	{
		registers_save.LATDbits.LATD10 = LATDbits.LATD10;
		data_msg.data_id = QS_LED_UART;
		data_msg.data1 = LATDbits.LATD10;
		// Envoi du message
		EVE_write_new_msg_QS(eve_global.bal_id_card_to_ihm_qs_data,data_msg);
	}
	// LED_USER
	if(registers_save.LATDbits.LATD11 != LATDbits.LATD11)
	{
		registers_save.LATDbits.LATD11 = LATDbits.LATD11;
		data_msg.data_id = QS_LED_USER;
		data_msg.data1 = LATDbits.LATD11;
		// Envoi du message
		EVE_write_new_msg_QS(eve_global.bal_id_card_to_ihm_qs_data,data_msg);
	}
	// LED_USER2
	if(registers_save.LATDbits.LATD12 != LATDbits.LATD12)
	{
		registers_save.LATDbits.LATD12 = LATDbits.LATD12;
		data_msg.data_id = QS_LED_USER2;
		data_msg.data1 = LATDbits.LATD12;
		// Envoi du message
		EVE_write_new_msg_QS(eve_global.bal_id_card_to_ihm_qs_data,data_msg);
	}
	// LED_ERROR
	if(registers_save.LATDbits.LATD13 != LATDbits.LATD13)
	{
		registers_save.LATDbits.LATD13 = LATDbits.LATD13;
		data_msg.data_id = QS_LED_ERROR;
		data_msg.data1 = LATDbits.LATD13;
		// Envoi du message
		EVE_write_new_msg_QS(eve_global.bal_id_card_to_ihm_qs_data,data_msg);
	}
}

// Fonction de vérification des changements d'état des PWM
void EVE_HARD_check_pwm()
{
	EVE_QS_data_msg_t data_msg;
	data_msg = EVE_init_msg_qs_data();

	// PWM 1
	if(LATEbits.LATE1 != 0)
	{
		LATEbits.LATE1 = 0;							// Remise à zéro du flag
		data_msg.data_id = QS_PWM;					// Référence des données PWM
		data_msg.data1 = PWM_CHANNEL_1;				// Numéro du canal
		data_msg.data2 = eve_global.pwm_power[0];	// Puissance de la PWM
		data_msg.data3 = LATEbits.LATE0;			// Sens de la PWM
		// Envoi du message
		EVE_write_new_msg_QS(eve_global.bal_id_card_to_ihm_qs_data,data_msg);
	}

	// PWM 2
	if(LATEbits.LATE3 != 0)
	{
		LATEbits.LATE3 = 0;							// Remise à zéro du flag
		data_msg.data_id = QS_PWM;					// Référence des données PWM
		data_msg.data1 = PWM_CHANNEL_2;				// Numéro du canal
		data_msg.data2 = eve_global.pwm_power[1];	// Puissance de la PWM
		data_msg.data3 = LATEbits.LATE2;			// Sens de la PWM
		// Envoi du message
		EVE_write_new_msg_QS(eve_global.bal_id_card_to_ihm_qs_data,data_msg);
	}

	// PWM 3
	if(LATEbits.LATE5 != 0)
	{
		LATEbits.LATE5 = 0;							// Remise à zéro du flag
		data_msg.data_id = QS_PWM;					// Référence des données PWM
		data_msg.data1 = PWM_CHANNEL_3;				// Numéro du canal
		data_msg.data2 = eve_global.pwm_power[2];	// Puissance de la PWM
		data_msg.data3 = LATEbits.LATE4;			// Sens de la PWM
		// Envoi du message
		EVE_write_new_msg_QS(eve_global.bal_id_card_to_ihm_qs_data,data_msg);
	}

	// PWM 4
	if(LATEbits.LATE7 != 0)
	{
		LATEbits.LATE7 = 0;							// Remise à zéro du flag
		data_msg.data_id = QS_PWM;					// Référence des données PWM
		data_msg.data1 = PWM_CHANNEL_4;				// Numéro du canal
		data_msg.data2 = eve_global.pwm_power[3];	// Puissance de la PWM
		data_msg.data3 = LATEbits.LATE6;			// Sens de la PWM
		// Envoi du message
		EVE_write_new_msg_QS(eve_global.bal_id_card_to_ihm_qs_data,data_msg);
	}
}

// Fonction permettant d'initialiser un message type QS_DATA
EVE_QS_data_msg_t EVE_init_msg_qs_data()
{
	EVE_QS_data_msg_t data_msg;

	data_msg.mtype = QUEUE_QS_DATA_MTYPE;
	data_msg.data_id = QS_UNDEFINED_DATA;
	data_msg.data1 = 0;
	data_msg.data2 = 0;
	data_msg.data3 = 0;

	return data_msg;
}

