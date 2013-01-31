/*
 *  Club Robot ESEO 2008 - 2010
 *  Game Hoover - Archi-Tech'
 *
 *  Fichier : QS_can.c
 *  Package : Qualité Soft
 *  Description : fonction CAN
 *  Auteur : Jacen
 *  Version 20090808
 */

#include "QS_all.h"
#include <can.h>

#ifdef TEST_MODE
#undef TEST_MODE
#define MATCH_MODE
#endif

/*
 *	Il y a CAN_NB modules CAN numérotés de FIRST_CAN_NB
 *	à (FIRST_CAN_NB+CAN_NB-1).
 */
#define CAN_NB				2
#define FIRST_CAN_NB		1

#define CAN_BUF_SIZE		16
#define CAN_FILTER_NB		6
#define CAN_MASK_NB			2
#define CAN_TX_BUFFER_NB	3
#define CAN_RX_BUF_NB		2

typedef void (*can_set_operation_mode_fun)(unsigned int config);
typedef void (*can_initialize_fun)(unsigned int config1, unsigned int config2);
typedef void (*can_config_int_fun)(unsigned int config1, unsigned int config2);
typedef void (*can_set_tx_mode_fun)(char buffno, unsigned int config);
typedef void (*can_set_rx_mode_fun)(char buffno, unsigned int config);
typedef char (*can_is_tx_ready_fun)(char buffno);
typedef char (*can_is_rx_full_fun)(char buffno);
typedef void (*can_send_message_fun)(unsigned int sid, unsigned long eid, unsigned char * data, unsigned char  datalen, char MsgFlag);
typedef void (*can_receive_message_fun)(unsigned char * data, unsigned char  datalen, char MsgFlag);

typedef struct
{
	CAN_msg_t buffer[CAN_BUF_SIZE];
	Uint8 next_to_read;
	volatile Uint8 next_to_write;

	/* cette variable permet de rajouter des filtres en réception pour recevoir
	plus de messages, un controle étant fait pour ne pas dépasser le nombre de
	filtres supporté par le module CAN.*/
	Uint8 custom_filter_set_nb;
}can_rx_buffer_t;

static can_rx_buffer_t Can_rx[CAN_NB];

typedef struct
{
	can_send_message_fun send_message;
	can_receive_message_fun receive_message;
	can_set_operation_mode_fun set_op_mode;
	can_initialize_fun initialize;
	can_config_int_fun config_int;
	can_set_tx_mode_fun set_tx_mode;
	can_set_rx_mode_fun set_rx_mode;
	can_is_tx_ready_fun is_tx_ready;
	can_is_rx_full_fun is_rx_full;
	volatile CxCTRLBITS *ctrl_bits;
	volatile CxINTFBITS *intf_bits;
	volatile CxRXxSIDBITS *rx_sid_bits[CAN_RX_BUF_NB];
	volatile CxRXxDLCBITS *rx_dlc_bits[CAN_RX_BUF_NB];
	volatile CxRX0CONBITS *rx_con0;
	volatile CxRX1CONBITS *rx_con1;
	volatile unsigned int *rx_filter[CAN_FILTER_NB];
	volatile unsigned int *rx_mask[CAN_MASK_NB];
	volatile unsigned int *ifs_reg;
	Uint8 if_bit_num;
	volatile unsigned int *pin_tris;
}can_t;

#warning valeur erronée de if_bit_num

static can_t Can[CAN_NB]=
{
(can_t){	CAN1SendMessage,
			CAN1ReceiveMessage,
			CAN1SetOperationMode,
			CAN1Initialize,
			ConfigIntCAN1,
			CAN1SetTXMode,
			CAN1SetRXMode,
			CAN1IsTXReady,
			CAN1IsRXReady,
			&C1CTRLbits,
			&C1INTFbits,
			{&C1RX0SIDbits, &C1RX1SIDbits},
			{&C1RX0DLCbits, &C1RX1DLCbits},
			&C1RX0CONbits, 
			&C1RX1CONbits,
			{&C1RXF0SID, &C1RXF1SID, &C1RXF2SID, &C1RXF3SID, &C1RXF4SID, &C1RXF5SID},
			{&C1RXM0SID, &C1RXM1SID},
			&IFS1,
			0,
			&TRISF
		},
(can_t){	CAN2SendMessage,
			CAN2ReceiveMessage,
			CAN2SetOperationMode,
			CAN2Initialize,
			ConfigIntCAN2,
			CAN2SetTXMode,
			CAN2SetRXMode,
			CAN2IsTXReady,
			CAN2IsRXReady,
			&C2CTRLbits,
			&C2INTFbits,
			{&C2RX0SIDbits, &C2RX1SIDbits},
			{&C2RX0DLCbits, &C2RX1DLCbits},
			&C2RX0CONbits,
			&C2RX1CONbits,
			{&C2RXF0SID, &C2RXF1SID, &C2RXF2SID, &C2RXF3SID, &C2RXF4SID, &C2RXF5SID},
			{&C2RXM0SID, &C2RXM1SID},
			&IFS2,
			0,
			&TRISG
		}
};

void CAN_init(	Uint8 canID, 
				Uint11 my_filter, Uint11 my_mask,
				Uint11 broadcast_filter, Uint11 broadcast_mask)
{
	canID-=FIRST_CAN_NB;
	assert(canID<CAN_NB);
	if(canID>=CAN_NB)
		canID = CAN_NB-1;
	Sint8 i;
	Uint16 CANModeOperating, CANModeConfig, CANInitialize, CANSettings, CANConfigInt, CANIntPriority;

	/************************************************************************/
	/*	Préparation des config 												*/
	/************************************************************************/
	
	CANModeConfig =	CAN_IDLE_CON &
					CAN_MASTERCLOCK_1 & //FCAN clock is Fcy
					CAN_REQ_OPERMODE_CONFIG &
					CAN_CAPTURE_DIS;
	
	CANInitialize =	CAN_SYNC_JUMP_WIDTH1 &
	#ifdef FREQ_40MHZ
					CAN_BAUD_PRE_SCALE(11);
	#elif defined(FREQ_20MHZ)
					CAN_BAUD_PRE_SCALE(5);
	#else
					CAN_BAUD_PRE_SCALE(2);
	#endif
	
	CANSettings =	CAN_WAKEUP_BY_FILTER_DIS &
					CAN_PHASE_SEG2_TQ(3) &
					CAN_PHASE_SEG1_TQ(3) &
					CAN_PROPAGATIONTIME_SEG_TQ(3) &
					CAN_SEG2_FREE_PROG &
					CAN_SAMPLE3TIMES;

	CANConfigInt =	
					CAN_INDI_INVMESS_DIS &
					CAN_INDI_WAK_DIS &
					CAN_INDI_ERR_DIS &
					CAN_INDI_TXB2_DIS &
					CAN_INDI_TXB1_DIS &
					CAN_INDI_TXB0_DIS &
					CAN_INDI_RXB1_EN &
					CAN_INDI_RXB0_EN;
	
	CANIntPriority=	CAN_INT_PRI_6 &
					CAN_INT_ENABLE;
	CANModeOperating =	CAN_IDLE_CON &
						CAN_MASTERCLOCK_1 &
						CAN_REQ_OPERMODE_NOR &
						CAN_CAPTURE_DIS;
	
	/************************************************************************/
	/*	Application des configs au module CAN demandé 						*/
	/************************************************************************/
	BIT_CLR(*(Can[canID].ifs_reg), Can[canID].if_bit_num);
	Can[canID].set_op_mode(CANModeConfig);
	while((Can[canID].ctrl_bits)->OPMODE <=3);
	
	Can[canID].initialize(CANInitialize, CANSettings);
	Can[canID].config_int(CANConfigInt, CANIntPriority);

	BIT_CLR(*(Can[canID].ifs_reg), Can[canID].if_bit_num);

	/*************************************************************************
	*	Configuration des masques 
	**************************************************************************/
	*(Can[canID].rx_mask[0])=broadcast_mask<<2;
	*(Can[canID].rx_mask[1])=my_mask<<2;
	/*************************************************************************
	*	Configuration des filtres 
	**************************************************************************/
	for(i=0; i<CAN_FILTER_NB; i++)
	{
		switch(i)
		{
			case 0: case 1:
				*(Can[canID].rx_filter[i])=broadcast_filter<<2;
				break;
			case 2:
			default:
				*(Can[canID].rx_filter[i])=my_filter<<2;
				break;
		}
	}
	Can_rx[canID].custom_filter_set_nb = 1;
	/*************************************************************************
	*	config de tous les registres de reception
	**************************************************************************/
	for(i=0; i<CAN_RX_BUF_NB; i++)
		Can[canID].set_rx_mode(i,CAN_RXFUL_CLEAR & CAN_BUF0_DBLBUFFER_DIS);

	/*************************************************************************
	*	config de tous les registres d'envoi
	**************************************************************************/
	for(i=0; i<CAN_TX_BUFFER_NB; i++)
		Can[canID].set_tx_mode(i,CAN_TX_STOP_REQ & CAN_TX_PRIORITY_HIGH );

	/*************************************************************************
	*	préparation du buffer circulaire de reception
	**************************************************************************/
	Can_rx[canID].next_to_read=0;
	Can_rx[canID].next_to_write=0;
	
	/* configuration des TRIS pour bon fonctionnement du module CAN */
	*(Can[canID].pin_tris) = (*(Can[canID].pin_tris) & 0xFFFC) | 0x0001;

	/* on bascule en mode operationnel */
	Can[canID].set_op_mode(CANModeOperating);
	while((Can[canID].ctrl_bits)->OPMODE !=0);
}


void CAN_send(Uint8 canID,CAN_msg_t* can_msg)
{
	canID-=FIRST_CAN_NB;
	assert(canID<CAN_NB);
	if(canID>=CAN_NB)
		canID = CAN_NB-1;
	Sint8 used_buffer;
	for(used_buffer=0; !(Can[canID].is_tx_ready(used_buffer)); used_buffer=(used_buffer+1)%3);
	
	Can[canID].send_message((CAN_TX_SID(can_msg->sid)) & (CAN_TX_EID_DIS) & (CAN_SUB_NOR_TX_REQ),
					(CAN_TX_EID(12344)) & (CAN_NOR_TX_REQ),can_msg->data, can_msg->size, used_buffer);
}

void CAN_add_custom_filter(Uint8 canID, Uint11 sid)
{
	canID-=FIRST_CAN_NB;
	assert(canID<CAN_NB);
	if(canID>=CAN_NB)
		canID = CAN_NB-1;
	if (Can_rx[canID].custom_filter_set_nb < (CAN_FILTER_NB -2))
	{
		*(Can[canID].rx_filter[2+Can_rx[canID].custom_filter_set_nb])=sid<<2;
		(Can_rx[canID].custom_filter_set_nb)++;
	}
}		

void CAN_add_broadcast_filter(Uint8 canID, Uint11 sid)
{
	canID-=FIRST_CAN_NB;
	assert(canID<CAN_NB);
	if(canID>=CAN_NB)
		canID = CAN_NB-1;
	/* quoi on affecte toujours le filtre excédentaire au meme registre */
	*(Can[canID].rx_filter[1])=sid<<2;
}		

static void CAN_receive(Uint8 canID,CAN_msg_t* can_msg)
{
	canID-=FIRST_CAN_NB;
	assert(canID<CAN_NB);
	if(canID>=CAN_NB)
		canID = CAN_NB-1;

	if ((Can[canID].rx_con0)->RXFUL==1)
	{
		/* réception d'un message dans le buffer RX0 */
		can_msg->sid= (Can[canID].rx_sid_bits[0])->SID;
		can_msg->size= (Can[canID].rx_dlc_bits[0])->DLC;
		Can[canID].receive_message(can_msg->data,can_msg->size,0);
		(Can[canID].rx_con0)->RXFUL=0;
		BIT_CLR(*(Can[canID].ifs_reg), Can[canID].if_bit_num);
	}
	else if((Can[canID].rx_con1)->RXFUL==1)
	{
		/* réception d'un message dans le buffer RX1 */
		can_msg->sid= (Can[canID].rx_sid_bits[1])->SID;
		can_msg->size= (Can[canID].rx_dlc_bits[1])->DLC;
		Can[canID].receive_message(can_msg->data,can_msg->size,1);
		(Can[canID].rx_con1)->RXFUL=0;
		BIT_CLR(*(Can[canID].ifs_reg), Can[canID].if_bit_num);
	}
	else 
	{
		assert(0);
		can_msg->sid=0;
		can_msg->size=0;
	}
	return;
}

bool_e CAN_got_new_message(Uint8 canID)
{
	canID-=FIRST_CAN_NB;
	assert(canID<CAN_NB);
	if(canID>=CAN_NB)
		canID = CAN_NB-1;
		return (Can_rx[canID].next_to_read != Can_rx[canID].next_to_write);
}
CAN_msg_t CAN_get_next_msg(Uint8 canID)
{
	canID-=FIRST_CAN_NB;
	assert(canID<CAN_NB);
	if(canID>=CAN_NB)
		canID = CAN_NB-1;
	assert(CAN_got_new_message(canID+FIRST_CAN_NB));

	CAN_msg_t* msgToReturn;
	/*	vérification déjà effectuée dans le assert, mais permet de prévenir des bugs
		une fois en match.*/
	if (Can_rx[canID].next_to_read != Can_rx[canID].next_to_write)
	{
		// récupérer le prochain message à lire
		msgToReturn = (Can_rx[canID].buffer + ((Can_rx[canID].next_to_read)++));
		Can_rx[canID].next_to_read %= CAN_BUF_SIZE;
		return *msgToReturn;
	}
	else
	{
		// renvoie d'un message invalide.
		return (CAN_msg_t) {0, "\0\0\0\0\0\0\0", 0};
	}	
}

/*
void CAN_error_processing()
{
	debug_printf("Error List :\r\n");
	if( C1TX0CONbits.TXABT||C1TX1CONbits.TXABT||C1TX2CONbits.TXABT)
		debug_printf("Error TXABT\r\n");
	if( C1TX0CONbits.TXLARB||C1TX1CONbits.TXLARB||C1TX2CONbits.TXLARB)
		debug_printf("Error TXLARB\r\n");
	if( C1TX0CONbits.TXERR||C1TX1CONbits.TXERR||C1TX2CONbits.TXERR)
		debug_printf("Error TXERR\r\n");
	if( C1INTFbits.RX0OVR || C1INTFbits.RX1OVR)
		debug_printf("Error RXOVR\r\n");
	if( C1INTFbits.TXBO)
		debug_printf("Error TXBO\r\n");
	if( C1INTFbits.TXEP)
		debug_printf("Error TXEP\r\n");
	if( C1INTFbits.TXWAR)
		debug_printf("Error TXWAR\r\n");
	if( C1INTFbits.RXWAR)
		debug_printf("Error RXWAR\r\n");
	if( C1INTFbits.EWARN)
		debug_printf("Error EWARN\r\n");
	if( C1INTFbits.IVRIF)
		debug_printf("Error IVRIF\r\n");
	if( C1INTFbits.WAKIF)
		debug_printf("Error WAKIF\r\n");
	if( C1INTFbits.ERRIF)
		debug_printf("Error ERRIF\r\n");
	if( C1INTFbits.TX2IF)
		debug_printf("Error TX2IF\r\n");
	if( C1INTFbits.TX1IF)
		debug_printf("Error TX1IF\r\n");
	if( C1INTFbits.TX0IF)
		debug_printf("Error TX0IF\r\n");
	if( C1INTFbits.RX1IF)
		debug_printf("Error RX1IF\r\n");
	if( C1INTFbits.RX0IF)
		debug_printf("Error RX0IF\r\n");
//	CAN_reinit();
	debug_printf("End Error List\r\n");
}
*/

/*void _ISR _C1Interrupt(void)
{		
	IFS1bits.C1IF=0;
	#define WAKE_UP_INTERRUPT	7
	#define RXB0_INTERRUPT		6
	#define RXB1_INTERRUPT		5
	#define TXB0_INTERRUPT		4
	#define TXB1_INTERRUPT		3
	#define TXB2_INTERRUPT		2
	#define ERROR_INTERRUPT		1
	#define NO_INTERRUPT		0
	
	#ifdef TEST_MODE
	switch( C1CTRLbits.ICODE )
	{
		case RXB0_INTERRUPT:
		case RXB1_INTERRUPT:
			CAN_receive(global.can_buffer + global.can_rx_num);	
			global.can_rx_num++;
			global.can_rx_num%=CAN_BUF_SIZE;		
			global.flags.canrx = TRUE;
			break;
		case ERROR_INTERRUPT:
			CAN_error_processing();
			break;
		default:
			debug_printf("default interrupt\n");
			break;
	}
	#else
	CAN_receive(1,global.can_buffer + global.can_rx_num);	
	global.can_rx_num++;
	global.can_rx_num%=CAN_BUF_SIZE;		
	global.flags.canrx = TRUE;
	#endif // def TEST_MODE
}
*/

void _ISR _C1Interrupt(void)
{		
	BIT_CLR(*(Can[0].ifs_reg), Can[0].if_bit_num);
	CAN_receive(1,Can_rx[0].buffer + Can_rx[0].next_to_write);	
	Can_rx[0].next_to_write++;
	Can_rx[0].next_to_write %= CAN_BUF_SIZE;
	if(Can_rx[0].next_to_write == Can_rx[0].next_to_read)
	{
		Can_rx[0].next_to_read++;
		Can_rx[0].next_to_read %= CAN_BUF_SIZE;
	}	
}


void _ISR _C2Interrupt(void)
{		
	BIT_CLR(*(Can[1].ifs_reg), Can[1].if_bit_num);
	CAN_receive(2,Can_rx[1].buffer + Can_rx[1].next_to_write);	
	Can_rx[1].next_to_write++;
	Can_rx[1].next_to_write %= CAN_BUF_SIZE;
	if(Can_rx[1].next_to_write == Can_rx[1].next_to_read)
	{
		Can_rx[1].next_to_read++;
		Can_rx[1].next_to_read %= CAN_BUF_SIZE;
	}	
}
