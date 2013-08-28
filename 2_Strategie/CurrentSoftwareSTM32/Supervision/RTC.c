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
 
#define QS_I2C_C
#include "RTC.h"
#ifdef USE_RTC
	//#include <i2c.h>
	#include "../QS/QS_i2c.h"
#endif
	#include "../QS/QS_CANmsgList.h"
	#include "../QS/QS_can.h"

// Le code assurant le dialogue avec la RTC provient du site www.embedded-code.com.
// La license gratuite impose de ne pas effacer le bloc de commentaire suivant.
// Merci à ce site pour nous avoir fait gagner du temps !
//		Modifications par rapport au fichier d'origine : 
//			- BYTE remplacés par Uint8
//			- portage pour dsPIC30 (voir header)
//****************************************************************************************
//****************************************************************************************
//  Project Name:		MAXIM DS1307 REAL TIME CLOCK DRIVER
//						C CODE FILE
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

	#define	DS1307_I2C_ADDRESS			0xd0


	//------ SELECT COMPILER ------
	//(Enable one of these only)
	//#define	RTC_USING_PIC18
	//#define	RTC_USING_PIC24
	//#define	RTC_USING_PIC30
	//#define	RTC_USING_PIC32
	#define RTC_USING_STM32

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

	#ifdef RTC_USING_STM32
		#define	RTC_I2C_START_I2C()					I2C_GenerateSTART(I2C2_I2C_HANDLE, ENABLE)										//Generate bus start condition
		#define	RTC_I2C_START_IN_PROGRESS_BIT		FALSE											//Bit indicating start is still in progress
		#define	RTC_I2C_RESTART_I2C()				I2C_GenerateSTART(I2C2_I2C_HANDLE, ENABLE)								//Generate bus restart condition
		#define	RTC_I2C_RESTART_IN_PROGRESS_BIT		FALSE									//Bit indicating re-start is still in progress
		#define	RTC_I2C_STOP_I2C()					I2C_GenerateSTOP(I2C2_I2C_HANDLE, ENABLE)										//Generate bus stop condition
		#define	RTC_I2C_STOP_IN_PROGRESS_BIT		FALSE									//Bit indicating Stop is still in progress
		#define	RTC_I2C_WRITE_BYTE(a)				I2C_SendData(I2C2_I2C_HANDLE,a)												//Write byte to I2C device
		#define	RTC_I2C_TX_IN_PROGRESS_BIT			(!I2C_GetFlagStatus(I2C2_I2C_HANDLE,I2C_FLAG_BTF)										//Bit indicating transmit byte is still in progress
		#define	RTC_I2C_ACK_NOT_RECEIVED_BIT		I2C_GetFlagStatus(I2C2_I2C_HANDLE,I2C_FLAG_AF)									//Bit that is high when ACK was not received
		//#define	RTC_I2C_READ_BYTE_START				I2CCONbits.RCEN = 1; while(I2CSTATbits.RBF == 0) ;	//Read byte from I2C device function (optional)
		#define	RTC_I2C_READ_BYTE					I2C_ReceiveData(I2C2_I2C_HANDLE)													//Read byte from I2C device function / result byte of RTC_I2C_READ_FUNCTION_START
		#define RTC_I2C_ACK()							I2C_AcknowledgeConfig(I2C2_I2C_HANDLE,ENABLE)			//Generate bus ACK condition
		#define RTC_I2C_NOT_ACK()						I2C_AcknowledgeConfig(I2C2_I2C_HANDLE,DISABLE)			//Generate bus Not ACK condition
		//#define	RTC_I2C_ACK_IN_PROGRESS_BIT			I2CCONbits.ACKEN										//Bit indicating ACK is still in progress
		#define	RTC_I2C_IDLE_I2C()					while (I2C_GetFlagStatus(I2C2_I2C_HANDLE,I2C_FLAG_BUSY))			//Test if I2C1 module is idle (wait until it is ready for next operation)
	#endif

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


// Code ajouté répondant à nos besoins spécifiques


void RTC_init(void) 
{
	#ifdef USE_RTC
	#ifdef USE_I2C2
		I2C_init();
	#endif // def USE_I2C
	#endif	//def USE_RTC
}

//////////////////////////////////////////////////////////////////////


//**********************************************
//**********************************************
//********** SET REAL TIME CLOCK TIME **********
//**********************************************
//**********************************************
//The fields are converted from binary to BCD before being sent to the RTC
//All fields are 0 - #, except day, date and month, which are 1 - #
//Pointers are used for comanilty with rtc_get_time
//Uses 24 hour clock, not 12 hour
Uint8 RTC_set_time  (Uint8 *seconds, Uint8 *minutes, Uint8 *hours, Uint8 *day, Uint8 *date, Uint8 *month, Uint8 *year)
{
	debug_printf("RTC : nouvelle date demandée : ");
	switch(*day)
	{
		case 1: debug_printf("lundi"); 			break;
		case 2: debug_printf("mardi"); 			break;
		case 3: debug_printf("mercredi"); 		break;
		case 4: debug_printf("jeudi");	 		break;
		case 5: debug_printf("vendredi"); 		break;
		case 6: debug_printf("samedi"); 		break;
		case 7: debug_printf("dimanche"); 		break;
		default: debug_printf("invalid_day"); break;
	}
	debug_printf(" %.2d/%.2d/20%.2d %.2d:%.2d:%.2d\n", *date,*month,*year,*hours,*minutes,*seconds);
	if (*seconds > 59 || *minutes > 59 || *hours > 23 || *day > 7 || *day == 0 || *date > 31 || *month > 12  || *year > 99)
	{
		debug_printf("RTC ERROR : La date demandée n'est pas cohérente. \n");
		return FALSE;
	}
	#ifdef USE_RTC
	Uint8 temp;
	Uint8 datas[9];

	datas[0] = 0x00;

	temp = (*seconds / 10);
	datas[1] = ((*seconds - (temp * 10)) + (temp << 4)) & 0x7f;	//Bit7 = enable oscillator

	temp = (*minutes / 10);
	datas[2] = (*minutes - (temp * 10)) + (temp << 4);

	temp = (*hours / 10);
	datas[3] = 	((*hours - (temp * 10)) + (temp << 4)) & 0x3f;		//Bit6 low = set format to 24 hour

	temp = (*day / 10);
	datas[4] = (*day - (temp * 10)) + (temp << 4);

	temp = (*date / 10);
	datas[5] = (*date - (temp * 10)) + (temp << 4);

	temp = (*month / 10);
	datas[6] = (*month - (temp * 10)) + (temp << 4);

	temp = (*year / 10);
	datas[7] = (*year - (temp * 10)) + (temp << 4);

	datas[8] = 0x00;

	return I2C2_write(DS1307_I2C_ADDRESS, datas, 9, TRUE);
	#else
		return FALSE;
	#endif

	
}


//**********************************************
//**********************************************
//********** GET REAL TIME CLOCK TIME **********
//**********************************************
//**********************************************
//Returns:
//	1 if sucessful, 0 if not.
//All fields are converted to binary values
//All fields are 0 - #, except day, date and month, which are 1 - #
//Uses 24 hour clock, not 12 hour
Uint8 RTC_get_time (Uint8 *seconds, Uint8 *minutes, Uint8 *hours, Uint8 *day, Uint8 *date, Uint8 *month, Uint8 *year)
{
	Uint8 datas[7];
	*seconds = 0;
	*minutes = 0;
	*hours = 0;
	*day = 0;
	*date = 1;
	*month = 1;
	*year = 0;
	#ifdef USE_RTC
	datas[0] = 0x00;	//@ du premier registre
	if(I2C2_write(DS1307_I2C_ADDRESS, datas, 1, FALSE))	//Condition de stop non envoyée...
	{
		if(I2C2_read(DS1307_I2C_ADDRESS, datas, 7))	//Le START sera donc ici un RESTART
		{
			*seconds 	= (datas[0] & 0x0f) + (((datas[0] & 0x70) >> 4) * 10);		//(Bit 7 is osc flag bit - dump)
			*minutes 	= (datas[1] & 0x0f) + (((datas[1] & 0x70) >> 4) * 10);
			*hours 		= (datas[2] & 0x0f) + (((datas[2] & 0x30) >> 4) * 10);
			*day 		= (datas[3] & 0x07);
			*date 		= (datas[4] & 0x0f) + (((datas[3] & 0x30) >> 4) * 10);
			*month 		= (datas[5] & 0x0f) + (((datas[5] & 0x10) >> 4) * 10);
			*year 		= (datas[6] & 0x0f) + ((datas[6] >> 4) * 10);
			return TRUE;
		}
	}
	#endif
	return FALSE;
}



void RTC_print_time(void)
{
	Uint8 seconds, minutes, hours, day, date, month, year;
	RTC_get_time(&seconds, &minutes, &hours, &day, &date, &month, &year);
	switch(day)
	{
		case 1: debug_printf("lundi"); 			break;
		case 2: debug_printf("mardi"); 			break;
		case 3: debug_printf("mercredi"); 		break;
		case 4: debug_printf("jeudi");	 		break;
		case 5: debug_printf("vendredi"); 		break;
		case 6: debug_printf("samedi"); 		break;
		case 7: debug_printf("dimanche"); 		break;
		default: debug_printf("invalid_day"); break;
	}	
	debug_printf(" %.2d/%.2d/20%.2d %.2d:%.2d:%.2d\n", date,month,year,hours,minutes,seconds);	
}	

void RTC_can_send(void)
{
	CAN_msg_t msg;
	msg.sid = DEBUG_RTC_TIME;
	msg.size = 7;
	RTC_get_time(&(msg.data[0]), &(msg.data[1]), &(msg.data[2]), &(msg.data[3]), &(msg.data[4]), &(msg.data[5]), &(msg.data[6]));			
	/*
					Uint8 secondes
					Uint8 minutes
					Uint8 hours
					Uint8 day
					Uint8 months
					Uint8 year	(11 pour 2011)
	*/		
	CAN_send(&msg);	
}	

