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
	#include "QS/QS_all.h"
	#include "QS/QS_CANmsgList.h"
	#include "QS/QS_can.h"
	
	// Le code assurant le dialogue avec la RTC provient du site www.embedded-code.com.
	// La license gratuite impose de ne pas effacer le bloc de commentaire suivant.
	// Merci � ce site pour nous avoir fait gagner du temps !
	//		Modifications par rapport au fichier d'origine : 
	//			- BYTE remplac�s par Uint8
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
	
	
	
	//*****************************
	//*****************************
	//********** DEFINES **********
	//*****************************
	//*****************************
	#ifndef RTC_DS1307_C_INIT		//Do only once the first time this file is used
	#define	RTC_DS1307_C_INIT
	
	#define	DS1307_I2C_ADDRESS			0xd0
	
	
	//------ SELECT COMPILER ------
	//(Enable one of these only)
	//#define	RTC_USING_PIC18
	//#define	RTC_USING_PIC24
	#define	RTC_USING_PIC30
	//#define	RTC_USING_PIC32
	
	
	#ifdef RTC_USING_PIC30
	//##################
	//##### PIC 30 #####
	//##################
	#define	RTC_I2C_START_I2C()					I2CCONbits.SEN = 1										//Generate bus start condition
	#define	RTC_I2C_START_IN_PROGRESS_BIT		I2CCONbits.SEN											//Bit indicating start is still in progress
	#define	RTC_I2C_RESTART_I2C()					I2CCONbits.RSEN = 1									//Generate bus restart condition
	#define	RTC_I2C_RESTART_IN_PROGRESS_BIT		I2CCONbits.RSEN										//Bit indicating re-start is still in progress
	#define	RTC_I2C_STOP_I2C()					I2CCONbits.PEN = 1										//Generate bus stop condition
	#define	RTC_I2C_STOP_IN_PROGRESS_BIT		I2CCONbits.PEN											//Bit indicating Stop is still in progress
	#define	RTC_I2C_WRITE_BYTE(a)				I2CTRN = a												//Write byte to I2C device
	#define	RTC_I2C_TX_IN_PROGRESS_BIT			I2CSTATbits.TRSTAT										//Bit indicating transmit byte is still in progress
	#define	RTC_I2C_ACK_NOT_RECEIVED_BIT		I2CSTATbits.ACKSTAT									//Bit that is high when ACK was not received
	#define	RTC_I2C_READ_BYTE_START				I2CCONbits.RCEN = 1; while(I2CSTATbits.RBF == 0) ;	//Read byte from I2C device function (optional)
	#define	RTC_I2C_READ_BYTE					I2CRCV													//Read byte from I2C device function / result byte of RTC_I2C_READ_FUNCTION_START
	#define RTC_I2C_ACK()							I2CCONbits.ACKDT = 0; I2CCONbits.ACKEN = 1			//Generate bus ACK condition
	#define RTC_I2C_NOT_ACK()						I2CCONbits.ACKDT = 1; I2CCONbits.ACKEN = 1			//Generate bus Not ACK condition
	#define	RTC_I2C_ACK_IN_PROGRESS_BIT			I2CCONbits.ACKEN										//Bit indicating ACK is still in progress
	#define	RTC_I2C_IDLE_I2C()					while ((I2CCON & 0x001F) | (I2CSTATbits.R_W))			//Test if I2C1 module is idle (wait until it is ready for next operation)
	
	#endif //#ifdef RTC_USING_PIC30
	
	
	#ifdef RTC_USING_PIC24
	//##################
	//##### PIC 24 #####
	//##################
	#define	RTC_I2C_START_I2C					I2C2CONbits.SEN = 1										//Generate bus start condition
	#define	RTC_I2C_START_IN_PROGRESS_BIT		I2C2CONbits.SEN											//Bit indicating start is still in progress
	#define	RTC_I2C_RESTART_I2C					I2C2CONbits.RSEN = 1									//Generate bus restart condition
	#define	RTC_I2C_RESTART_IN_PROGRESS_BIT		I2C2CONbits.RSEN										//Bit indicating re-start is still in progress
	#define	RTC_I2C_STOP_I2C					I2C2CONbits.PEN = 1										//Generate bus stop condition
	#define	RTC_I2C_STOP_IN_PROGRESS_BIT		I2C2CONbits.PEN											//Bit indicating Stop is still in progress
	#define	RTC_I2C_WRITE_BYTE(a)				I2C2TRN = a												//Write byte to I2C device
	#define	RTC_I2C_TX_IN_PROGRESS_BIT			I2C2STATbits.TRSTAT										//Bit indicating transmit byte is still in progress
	#define	RTC_I2C_ACK_NOT_RECEIVED_BIT		I2C2STATbits.ACKSTAT									//Bit that is high when ACK was not received
	#define	RTC_I2C_READ_BYTE_START				I2C2CONbits.RCEN = 1; while(I2C2STATbits.RBF == 0) ;	//Read byte from I2C device function (optional)
	#define	RTC_I2C_READ_BYTE					I2C2RCV													//Read byte from I2C device function / result byte of RTC_I2C_READ_FUNCTION_START
	#define RTC_I2C_ACK							I2C2CONbits.ACKDT = 0; I2C2CONbits.ACKEN = 1			//Generate bus ACK condition
	#define RTC_I2C_NOT_ACK						I2C2CONbits.ACKDT = 1; I2C2CONbits.ACKEN = 1			//Generate bus Not ACK condition
	#define	RTC_I2C_ACK_IN_PROGRESS_BIT			I2C2CONbits.ACKEN										//Bit indicating ACK is still in progress
	#define	RTC_I2C_IDLE_I2C					while ((I2C2CON & 0x001F) | (I2C2STATbits.R_W))			//Test if I2C1 module is idle (wait until it is ready for next operation)
	
	#endif //#ifdef RTC_USING_PIC24
	
	
	#ifdef RTC_USING_PIC32
	//##################
	//##### PIC 32 #####
	//##################
	#define	RTC_I2C_START_I2C					StartI2C1					//Generate bus start condition
	#define	RTC_I2C_START_IN_PROGRESS_BIT		I2C1CONbits.SEN				//Bit indicating start is still in progress
	#define	RTC_I2C_RESTART_I2C					RestartI2C1					//Generate bus restart condition
	#define	RTC_I2C_RESTART_IN_PROGRESS_BIT		I2C1CONbits.RSEN			//Bit indicating re-start is still in progress
	#define	RTC_I2C_STOP_I2C					StopI2C1					//Generate bus stop condition
	#define	RTC_I2C_STOP_IN_PROGRESS_BIT		I2C1CONbits.PEN				//Bit indicating Stop is still in progress
	#define	RTC_I2C_WRITE_BYTE(a)				MasterWriteI2C1(a)			//Write byte to I2C device
	#define	RTC_I2C_TX_IN_PROGRESS_BIT			I2C1STATbits.TRSTAT			//Bit indicating transmit byte is still in progress
	#define	RTC_I2C_ACK_NOT_RECEIVED_BIT		I2C1STATbits.ACKSTAT		//Bit that is high when ACK was not received
	//#define	RTC_I2C_READ_BYTE_START										//Read byte from I2C device function (optional)
	#define	RTC_I2C_READ_BYTE					MasterReadI2C1()			//Read byte from I2C device function / result byte of RTC_I2C_READ_FUNCTION_START
	#define RTC_I2C_ACK							AckI2C1						//Generate bus ACK condition
	#define RTC_I2C_NOT_ACK						NotAckI2C1					//Generate bus Not ACK condition
	#define	RTC_I2C_ACK_IN_PROGRESS_BIT			I2C1CONbits.ACKEN			//Bit indicating ACK is still in progress
	#define	RTC_I2C_IDLE_I2C					IdleI2C1					//Test if I2C1 module is idle (wait until it is ready for next operation)
	
	#endif //#ifdef RTC_USING_PIC32
	
	
	
	#endif //RTC_DS1307_C_INIT
	
	
	
	//*******************************
	//*******************************
	//********** FUNCTIONS **********
	//*******************************
	//*******************************
	#ifdef RTC_DS1307_C
	//-----------------------------------
	//----- INTERNAL ONLY FUNCTIONS -----
	//-----------------------------------
	
	
	//-----------------------------------------
	//----- INTERNAL & EXTERNAL FUNCTIONS -----
	//-----------------------------------------
	//(Also defined below as extern)
	//unsigned char flash_read(unsigned long address);
	Uint8 RTC_set_time  (Uint8 *seconds, Uint8 *minutes, Uint8 *hours, Uint8 *day, Uint8 *date, Uint8 *month, Uint8 *year);
	Uint8 RTC_get_time (Uint8 *seconds, Uint8 *minutes, Uint8 *hours, Uint8 *day, Uint8 *date, Uint8 *month, Uint8 *year);
	
	
	#else
	//------------------------------
	//----- EXTERNAL FUNCTIONS -----
	//------------------------------
	//extern unsigned char flash_read(unsigned long address);
	
	
	/*
	@brief Fonction qui �crit dans la RTC les date et heure pass�s en param�tres
	@param	pointeurs vers les item d�crits
	@return 1 en cas de r�ussite, 0 en cas d'�chec.
	*/
	extern Uint8 RTC_set_time  (Uint8 *seconds, Uint8 *minutes, Uint8 *hours, Uint8 *day, Uint8 *date, Uint8 *month, Uint8 *year);
	
	/*
	@brief Fonction qui lit dans la RTC les date et heure pass�s en param�tres
	@param	pointeurs vers les item d�crits
	@return 1 en cas de r�ussite, 0 en cas d'�chec.
	@post	les valeurs sont remplies avec des 0 en cas d'�chec. (sauf date et mois qui sont mis � 1)
	*/
	extern Uint8 RTC_get_time (Uint8 *seconds, Uint8 *minutes, Uint8 *hours, Uint8 *day, Uint8 *date, Uint8 *month, Uint8 *year);
	
	
	#endif

	/*
	@brief 	Envoi le message SUPER_RTC_TIME avec les arguments tels que d�finis dans QSMsgDoc.
	@post 	Si le temps est lu comme invalide, les champs sont � 0 sauf la date et le mois qui sont � 1.
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
