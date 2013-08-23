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
	#include <i2c.h>
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


// Code ajouté répondant à nos besoins spécifiques


void RTC_init(void) 
{
	#ifdef USE_RTC
	#ifdef USE_I2C
		// Baud rate is set for 50 Khz 
		Uint16 config2 = 200;
		  // Configure I2C for 7 bit address mode 
		Uint16 config1 = (	I2C_ON & I2C_IDLE_CON & I2C_CLK_HLD &
					I2C_IPMI_DIS & I2C_7BIT_ADD &
					I2C_SLW_DIS & I2C_SM_DIS &
					I2C_GCALL_DIS & I2C_STR_DIS &
					I2C_NACK & I2C_ACK_DIS & I2C_RCV_DIS &
					I2C_STOP_DIS & I2C_RESTART_DIS &
					I2C_START_DIS);
		OpenI2C(config1,config2);
		ConfigIntI2C(MI2C_INT_OFF & MI2C_INT_PRI_3
					& SI2C_INT_OFF & SI2C_INT_PRI_5);
//		m_s1rnum=0;	
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
	#ifdef USE_RTC
	Uint8 temp;
	Uint8 temp1;

	//Send the start condition
	RTC_I2C_IDLE_I2C();
	RTC_I2C_START_I2C();
	while (RTC_I2C_START_IN_PROGRESS_BIT);

	//Send the address with the write bit set
	RTC_I2C_IDLE_I2C();
	RTC_I2C_WRITE_BYTE(DS1307_I2C_ADDRESS & 0xfe);	//Bit 0 low for write
	while (RTC_I2C_TX_IN_PROGRESS_BIT)
		;
	if (RTC_I2C_ACK_NOT_RECEIVED_BIT)
		goto rtc_set_time_fail;

	//Send the register address
	RTC_I2C_IDLE_I2C();
	RTC_I2C_WRITE_BYTE(0x00);
	while (RTC_I2C_TX_IN_PROGRESS_BIT)
		;
	if (RTC_I2C_ACK_NOT_RECEIVED_BIT)
		goto rtc_set_time_fail;

	//Write seconds
	if (*seconds > 59)						//Ensure value is in range
		goto rtc_set_time_fail;
	temp1 = (*seconds / 10);
	temp = (*seconds - (temp1 * 10)) + (temp1 << 4);
	temp &= 0x7f;							//Bit7 = enable oscillator
	RTC_I2C_IDLE_I2C();
	RTC_I2C_WRITE_BYTE(temp);
	while (RTC_I2C_TX_IN_PROGRESS_BIT)
		;
	if (RTC_I2C_ACK_NOT_RECEIVED_BIT)
		goto rtc_set_time_fail;

	//Write minutes
	if (*minutes > 59)						//Ensure value is in range
		goto rtc_set_time_fail;

	temp1 = (*minutes / 10);
	temp = (*minutes - (temp1 * 10)) + (temp1 << 4);
	RTC_I2C_IDLE_I2C();
	RTC_I2C_WRITE_BYTE(temp);
	while (RTC_I2C_TX_IN_PROGRESS_BIT)
		;
	if (RTC_I2C_ACK_NOT_RECEIVED_BIT)
		goto rtc_set_time_fail;

	//Write hours
	if (*hours > 23)					//Ensure value is in range
		goto rtc_set_time_fail;

	temp1 = (*hours / 10);
	temp = (*hours - (temp1 * 10)) + (temp1 << 4);
	temp &= 0x3f;						//Bit6 low = set format to 24 hour
	RTC_I2C_IDLE_I2C();
	RTC_I2C_WRITE_BYTE(temp);
	while (RTC_I2C_TX_IN_PROGRESS_BIT)
		;
	if (RTC_I2C_ACK_NOT_RECEIVED_BIT)
		goto rtc_set_time_fail;

	//Write day
	if (*day > 7)						//Ensure value is in range
		goto rtc_set_time_fail;
	if (*day == 0)
		goto rtc_set_time_fail;

	temp1 = (*day / 10);
	temp = (*day - (temp1 * 10)) + (temp1 << 4);
	RTC_I2C_IDLE_I2C();
	RTC_I2C_WRITE_BYTE(temp);
	while (RTC_I2C_TX_IN_PROGRESS_BIT)
		;
	if (RTC_I2C_ACK_NOT_RECEIVED_BIT)
		goto rtc_set_time_fail;

	//Write date
	if (*date > 31)						//Ensure value is in range
		goto rtc_set_time_fail;

	temp1 = (*date / 10);
	temp = (*date - (temp1 * 10)) + (temp1 << 4);
	RTC_I2C_IDLE_I2C();
	RTC_I2C_WRITE_BYTE(temp);
	while (RTC_I2C_TX_IN_PROGRESS_BIT)
		;
	if (RTC_I2C_ACK_NOT_RECEIVED_BIT)
		goto rtc_set_time_fail;

	//Write month
	if (*month > 12)					//Ensure value is in range
		goto rtc_set_time_fail;

	temp1 = (*month / 10);
	temp = (*month - (temp1 * 10)) + (temp1 << 4);
	RTC_I2C_IDLE_I2C();
	RTC_I2C_WRITE_BYTE(temp);
	while (RTC_I2C_TX_IN_PROGRESS_BIT)
		;
	if (RTC_I2C_ACK_NOT_RECEIVED_BIT)
		goto rtc_set_time_fail;

	//Write year
	if (*year > 99)						//Ensure value is in range
		goto rtc_set_time_fail;

	temp1 = (*year / 10);
	temp = (*year - (temp1 * 10)) + (temp1 << 4);
	RTC_I2C_IDLE_I2C();
	RTC_I2C_WRITE_BYTE(temp);
	while (RTC_I2C_TX_IN_PROGRESS_BIT)
		;
	if (RTC_I2C_ACK_NOT_RECEIVED_BIT)
		goto rtc_set_time_fail;

	//Write control
	RTC_I2C_IDLE_I2C();
	RTC_I2C_WRITE_BYTE(0x00);				//0x00 = square wave output off
	while (RTC_I2C_TX_IN_PROGRESS_BIT)
		;
	if (RTC_I2C_ACK_NOT_RECEIVED_BIT)
		goto rtc_set_time_fail;

	//Send Stop
	RTC_I2C_IDLE_I2C();
	RTC_I2C_STOP_I2C();
	while (RTC_I2C_STOP_IN_PROGRESS_BIT)
		;
	
	return (1);

//----- I2C COMMS FAILED -----
rtc_set_time_fail:

	//Send Stop
	RTC_I2C_IDLE_I2C();
	RTC_I2C_STOP_I2C();
	while (RTC_I2C_STOP_IN_PROGRESS_BIT)
		;
	#endif
	return (0);
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
	#ifdef USE_RTC
	Uint8 temp;


	//Send the start condition
	RTC_I2C_IDLE_I2C();
	RTC_I2C_START_I2C();
	while (RTC_I2C_START_IN_PROGRESS_BIT)
		;

	//Send the address with the write bit set
	RTC_I2C_IDLE_I2C();
	RTC_I2C_WRITE_BYTE(DS1307_I2C_ADDRESS & 0xfe);	//Bit 0 low for write
	while (RTC_I2C_TX_IN_PROGRESS_BIT)
		;
	if (RTC_I2C_ACK_NOT_RECEIVED_BIT)
		goto rtc_get_time_fail;

	//Send the register address
	RTC_I2C_IDLE_I2C();
	RTC_I2C_WRITE_BYTE(0x00);
	while (RTC_I2C_TX_IN_PROGRESS_BIT)
		;
	if (RTC_I2C_ACK_NOT_RECEIVED_BIT)
		goto rtc_get_time_fail;

	//Send restart condition
	RTC_I2C_IDLE_I2C();
	RTC_I2C_RESTART_I2C();
	while (RTC_I2C_RESTART_IN_PROGRESS_BIT)
		;

	//Send the address with the read bit set
	RTC_I2C_IDLE_I2C();
	RTC_I2C_WRITE_BYTE(DS1307_I2C_ADDRESS | 0x01);	//Bit 1 high for read
	while (RTC_I2C_TX_IN_PROGRESS_BIT)
		;
	if (RTC_I2C_ACK_NOT_RECEIVED_BIT)
		goto rtc_get_time_fail;

	//Read seconds
	RTC_I2C_IDLE_I2C();
	#ifdef RTC_I2C_READ_BYTE_START
		RTC_I2C_READ_BYTE_START
	#endif
	temp = RTC_I2C_READ_BYTE;
	*seconds = (temp & 0x0f) + (((temp & 0x70) >> 4) * 10);		//(Bit 7 is osc flag bit - dump)
	RTC_I2C_ACK();					//Send Ack
	while (RTC_I2C_ACK_IN_PROGRESS_BIT)
		;

	//Read minutes
	RTC_I2C_IDLE_I2C();
	#ifdef RTC_I2C_READ_BYTE_START
		RTC_I2C_READ_BYTE_START
	#endif
	temp = RTC_I2C_READ_BYTE;
	*minutes = (temp & 0x0f) + (((temp & 0x70) >> 4) * 10);
	RTC_I2C_ACK();					//Send Ack
	while (RTC_I2C_ACK_IN_PROGRESS_BIT)
		;

	//Read hours
	RTC_I2C_IDLE_I2C();
	#ifdef RTC_I2C_READ_BYTE_START
		RTC_I2C_READ_BYTE_START
	#endif
	temp = RTC_I2C_READ_BYTE;
	*hours = (temp & 0x0f) + (((temp & 0x30) >> 4) * 10);
	RTC_I2C_ACK();					//Send Ack
	while (RTC_I2C_ACK_IN_PROGRESS_BIT)
		;

	//Read day
	RTC_I2C_IDLE_I2C();
	#ifdef RTC_I2C_READ_BYTE_START
		RTC_I2C_READ_BYTE_START
	#endif
	temp = RTC_I2C_READ_BYTE;
	*day = (temp & 0x07);
	RTC_I2C_ACK();					//Send Ack
	while (RTC_I2C_ACK_IN_PROGRESS_BIT)
		;

	//Read date
	RTC_I2C_IDLE_I2C();
	#ifdef RTC_I2C_READ_BYTE_START
		RTC_I2C_READ_BYTE_START
	#endif
	temp = RTC_I2C_READ_BYTE;
	*date = (temp & 0x0f) + (((temp & 0x30) >> 4) * 10);
	RTC_I2C_ACK();					//Send Ack
	while (RTC_I2C_ACK_IN_PROGRESS_BIT)
		;

	//Read month
	RTC_I2C_IDLE_I2C();
	#ifdef RTC_I2C_READ_BYTE_START
		RTC_I2C_READ_BYTE_START
	#endif
	temp = RTC_I2C_READ_BYTE;
	*month = (temp & 0x0f) + (((temp & 0x10) >> 4) * 10);
	RTC_I2C_ACK();					//Send Ack
	while (RTC_I2C_ACK_IN_PROGRESS_BIT)
		;

	//Read year
	RTC_I2C_IDLE_I2C();
	#ifdef RTC_I2C_READ_BYTE_START
		RTC_I2C_READ_BYTE_START
	#endif
	temp = RTC_I2C_READ_BYTE;
	*year = (temp & 0x0f) + ((temp >> 4) * 10);
	//RTC_I2C_ACK();					//Send Ack
	//while (RTC_I2C_ACK_IN_PROGRESS_BIT)
	//	;

	//Send NAK
	RTC_I2C_IDLE_I2C();
	RTC_I2C_NOT_ACK();
	while (RTC_I2C_ACK_IN_PROGRESS_BIT)
		;

	//Send Stop
	RTC_I2C_IDLE_I2C();
	RTC_I2C_STOP_I2C();
	while (RTC_I2C_STOP_IN_PROGRESS_BIT)
		;
	
	return (1);

//----- I2C COMMS FAILED -----
rtc_get_time_fail:

	*seconds = 0;
	*minutes = 0;
	*hours = 0;
	*day = 0;
	*date = 1;
	*month = 1;
	*year = 0;

	//Send Stop
	RTC_I2C_IDLE_I2C();
	RTC_I2C_STOP_I2C();
	while (RTC_I2C_STOP_IN_PROGRESS_BIT)
		;
	#else
		*seconds = 0;
		*minutes = 0;
		*hours = 0;
		*day = 0;
		*date = 1;
		*month = 1;
		*year = 0;
	#endif
	return (0);
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

