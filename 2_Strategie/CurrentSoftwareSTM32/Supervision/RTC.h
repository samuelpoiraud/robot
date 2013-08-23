/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : RTC.c
 *	Package : Supervision
 *	Description : Gestion de la RTC I2C sur dsPIC30F6010A
 *	Auteur : Nirgal
 *  Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20110518
 */
 
 #ifndef RTC_H
	#define RTC_H
	#include "../QS/QS_all.h"

	
	// Le code assurant le dialogue avec la RTC provient du site www.embedded-code.com.
	// La license gratuite impose de ne pas effacer le bloc de commentaire suivant.
	// Merci à ce site pour nous avoir fait gagner du temps !
	//		Modifications par rapport au fichier d'origine : 
	//			- BYTE remplacés par Uint8
	//			- portage pour dsPIC30
	
	//****************************************************************************************
	//****************************************************************************************
	//  Project Name:		MAXIM DS1307 REAL TIME CLOCK DRIVER
	//						C CODE HEADER FILE
	//  Copyright:			EMBEDDED-CODE.COM
	//
	//  IMPORTANT: These files are copyright of embedded-code.com and are subject to a licence
	//  agreement.  All rights reserved.  Unauthorised use, reproduction or distribution of
	//  these files may lead to prosecution.
	//  Any use in violation of the licence restrictions may subject the user to criminal
	//  sanctions under applicable laws, as well as to civil liability for the breach of the
	//  terms and conditions of the license.
	//  This software is provided in an "as is" condition. No warranties, whether express,
	//  implied or statutory, including, but not limited to, implied warranties of
	//  merchantability and fitness for a particular purpose apply to this software. 
	//  Embedded-code.com shall not be liable in any circumstances for special, incidental
	//  or consequential damages, for any reason whatsoever.
	//  Please see www.embedded-code.com/licence.php for full details.
	//  DO NOT REMOVE THIS NOTICE - IT IS A REQUIREMENT OF THE LICENCE AGREEMENT.
	//****************************************************************************************
	//****************************************************************************************
	
	
	//*************************************
	//*************************************
	//********** DRIVER REVISION **********
	//*************************************
	//*************************************
	//
	//V1.00
	//- Original release
	//
	//V1.01
	//- AckI2C() replaced with define RTC_I2C_ACK(), and NotAckI2C1() replaced with define RTC_I2C_NOT_ACK()
	//
	//V1.02
	//- Defines updated to generic I2C
	
	//##### USING THIS DRIVER #####
	//Include this header file in any .c files within your project from which you wish to use it's functions.
	
	
	//########## SETUP I2C IN THE MAIN POWERUP INIT ROUTINE ##########
	/*
	// ***** SETUP SSP AS I2C *****
	//I2C requirements:- 100kHz clock high time 4.0uS, clock low time 4.7uS, 400kHz clock high time 0.6uS, clock low time 1.3uS
	//The PIC18 does not fully conform to the 400kHz I2C spec (which applies to all rates >100kHz) but may be used with care
	//where higher rates are required.
	//Lower 7 bits of SSPADD are the baud rate.  ((Tcycle / 2) * SSPADD * 2) = time per bit.  1 / time per bit = frequency.
	//400kHz @ 40MHz OSC = SSPADD 25
	//100kHz @ 40MHz OSC = SSPADD 100
	//100kHz @ 32MHz OSC = SSPADD 80
	//100kHz @ 4MHz OSC = SSPADD 10
		SSPADD = 100;					//setup i2c clk speed
		OpenI2C(MASTER, SLEW_OFF);		//Slew rate control should be on for 400KHz, off for 100KHz & 1MHz
	*/
	
	
	


	//------------------------------
	//----- EXTERNAL FUNCTIONS -----
	//------------------------------
	//extern unsigned char flash_read(unsigned long address);
	
	
	/*
	@brief Fonction qui écrit dans la RTC les date et heure passés en paramètres
	@param	pointeurs vers les item décrits
	@return 1 en cas de réussite, 0 en cas d'échec.
	*/
	extern Uint8 RTC_set_time  (Uint8 *seconds, Uint8 *minutes, Uint8 *hours, Uint8 *day, Uint8 *date, Uint8 *month, Uint8 *year);
	
	/*
	@brief Fonction qui lit dans la RTC les date et heure passés en paramètres
	@param	pointeurs vers les item décrits
	@return 1 en cas de réussite, 0 en cas d'échec.
	@post	les valeurs sont remplies avec des 0 en cas d'échec. (sauf date et mois qui sont mis à 1)
	*/
	extern Uint8 RTC_get_time (Uint8 *seconds, Uint8 *minutes, Uint8 *hours, Uint8 *day, Uint8 *date, Uint8 *month, Uint8 *year);
	
	

	/*
	@brief 	Envoi le message DEBUG_RTC_TIME avec les arguments tels que définis dans QSMsgDoc.
	@post 	Si le temps est lu comme invalide, les champs sont à 0 sauf la date et le mois qui sont à 1.
	*/
	void RTC_can_send(void);

	/*
	@brief 	fonction d'init de la RTC (initialise le bus I2C !)
	*/void RTC_init(void);
	
	/*
	@brief	affiche en printf la date et l'heure actuels. Voir la fonction pour plus d'infos.
	*/
	void RTC_print_time(void);
	
	
	
#endif /* ndef RTC_H */
