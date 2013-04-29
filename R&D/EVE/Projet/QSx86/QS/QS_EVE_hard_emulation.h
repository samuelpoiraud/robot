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

#ifndef QS_EVE_HARD_EMULATION_H
	#define QS_EVE_HARD_EMULATION_H

	#include "QS_all.h"

	// On indique que les fonctions _ISR peuvent ne pas être utilisées
	// Cela permet d'empêcher le blocage de la compilation en supprimant '_ISR'
	// cf : http://gcc.gnu.org/onlinedocs/gcc-2.95.3/gcc_4.html#SEC84
	#define _ISR __attribute__((unused))

	// Modification de la fonction Nop()
	#define Nop()	nop()

	// Fonctions spéciales pour les interruptions d'arrêt du dsPIC (fonctions dans le main de chaque carte)
	// Ca ne sert à rien pour EVE ^^
	Uint16 rcon_dsPIC;
	#define RCON rcon_dsPIC
	Uint16 matherr_dsPIC;
	#define _MATHERR	matherr_dsPIC
	Uint16 stkerr_dsPIC;
	#define _STKERR		stkerr_dsPIC
	Uint16 addrerr_dsPIC;
	#define _ADDRERR	addrerr_dsPIC
	Uint16 oscfail_dsPIC;
	#define _OSCFAIL	oscfail_dsPIC
	
	Uint16 _C1IE_dsPIC;
	#define _C1IE _C1IE_dsPIC

	/************************** REDEFINITION DES PORTS **************************************/
	
	//Port B
	typedef struct
	{
		Uint8 RB0;
		Uint8 RB1;
		Uint8 RB2;
		Uint8 RB3;
		Uint8 RB4;
		Uint8 RB5;
		Uint8 RB6;
		Uint8 RB7;
		Uint8 RB8;
		Uint8 RB9;
		Uint8 RB10;
		Uint8 RB11;
		Uint8 RB12;
		Uint8 RB13;
		Uint8 RB14;
		Uint8 RB15;
	} PORTBbits_t;
	PORTBbits_t PORTBbits;
	
	//Port E
	typedef struct
	{
		Uint8 RE0;
		Uint8 RE1;
		Uint8 RE2;
		Uint8 RE3;
		Uint8 RE4;
		Uint8 RE5;
		Uint8 RE6;
		Uint8 RE7;
		Uint8 RE8;
		Uint8 RE9;
		Uint8 RE10;
		Uint8 RE11;
		Uint8 RE12;
		Uint8 RE13;
		Uint8 RE14;
		Uint8 RE15;
	} PORTEbits_t;
	PORTEbits_t PORTEbits;

	// Structure des ports des LEDs
	typedef struct
	{
		Uint8 LATD8;
		Uint8 LATD9;
		Uint8 LATD10;
		Uint8 LATD11;
		Uint8 LATD12;
		Uint8 LATD13;
	} LATDbits_t;

	// Structure des ports des boutons
	typedef struct
	{
		Uint8 RD4;
		Uint8 RD5;
		Uint8 RD6;
		Uint8 RD7;
	} PORTDbits_t;

	// Ports des LEDs
	LATDbits_t LATDbits;
	// Ports des boutons
	PORTDbits_t PORTDbits;

	// Ports LAT E
	typedef struct
	{
		Uint8 LATE0;
		Uint8 LATE1;
		Uint8 LATE2;
		Uint8 LATE3;
		Uint8 LATE4;
		Uint8 LATE5;
		Uint8 LATE6;
		Uint8 LATE7;
		Uint8 LATE8;
		Uint8 LATE9;
	} LATEbits_t;

	LATEbits_t LATEbits;

	// Ports G
	typedef struct
	{
		Uint16 LATG6;
		Uint16 LATG7;
	} LATGbits_t;
	typedef struct
	{
		Uint16 RG8;
	} PORTGbits_t;

	LATGbits_t LATGbits;
	PORTGbits_t PORTGbits;

	// IFS0
	typedef struct
	{
		Uint16 T1IF;
		Uint16 T2IF;
		Uint16 T3IF;
		Uint16 T4IF;
	} IFS0bits_t;

	IFS0bits_t IFS0bits;

	// Suppléments Prop
	volatile Uint8 PSVPAG;
	#define __builtin_psvpage(value)	0
	typedef struct
	{
		Uint16 INT1IP;
	} IPC4bits_t;
	IPC4bits_t IPC4bits;
	typedef struct
	{
		Uint16 INT2IP;
	} IPC5bits_t;
	IPC5bits_t IPC5bits;
	typedef struct
	{
		Uint16 PSV;
	} CORCONbits_t;
	CORCONbits_t CORCONbits;

	// Structure de sauvegarde de l'état des ports pour un envoi des informations uniquement lorsqu'un changement d'état se produit
	typedef struct
	{
		LATDbits_t LATDbits;
		// PORTDbits_t PORTDbits; Ports des Boutons en entrée, pas de sauvegarde nécessaire
		//LATEbits_t LATEbits;	// Ports des PWM 0 à 7
		LATGbits_t LATGbits;
		PORTGbits_t PORTGbits;
		//IFS0bits_t IFS0bits;
	} registers_save_t;


	// Fonction d'initialisation des variables liées au hard du dsPIC
	void EVE_HARD_init();

	// Fonction de vérification des changements d'états des variables liées aux ports
	// A chaque modification de l'ancien état
	void EVE_HARD_check_ports_modifications();

	// Fonction de vérification des changements d'état des LEDs
	void EVE_HARD_check_leds();

	// Fonction de vérification des changements d'état des PWM
	void EVE_HARD_check_pwm();

	// Fonction permettant d'initialiser un message type QS_DATA
	// return : une structure type EVE_QS_data_msg_t correctement initialisée
	EVE_QS_data_msg_t EVE_init_msg_qs_data();

	// Fonctions d'IT qui sont définies spécifiquement dans chaque carte
	#ifdef USE_T1Interrupt
		extern void _ISR _T1Interrupt();
	#endif /* USE_T1Interrupt */
	#ifdef USE_T2Interrupt
		extern void _ISR _T2Interrupt();
	#endif /* USE_T2Interrupt */
	#ifdef USE_T3Interrupt
		extern void _ISR _T3Interrupt();
	#endif/* USE_T3Interrupt */
	#ifdef USE_T4Interrupt
		extern void _ISR _T4Interrupt();
	#endif /* USE_T4Interrupt */

#endif /* QS_EVE_HARD_EMULATION */

