/*
 *  Club Robot ESEO 2006 - 2009
 *  Game Hoover - Archi-Tech'
 *
 *  Fichier : QS_can.c
 *  Package : Qualité Soft
 *  Description : fonction CAN
 *  Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 27032011
 */

#include "QS_can.h"

#include <can.h>
#include "../QS/QS_CANmsgList.h"
#define MASK_CAN_S			0x000 /* 0b00000000000 */
/* on filtre les messages nous meme en aval */

#if defined I_AM_CARTE_ASSER	//if (I_AM == CARTE_ASSER)
	#define MY_FILTER	ASSER_FILTER
#elif defined I_AM_CARTE_ACT	//(I_AM == CARTE_ACT)
	#define MY_FILTER	ACT_FILTER
#elif defined I_AM_CARTE_STRAT	//(I_AM == CARTE_STRAT)
	#define MY_FILTER	STRAT_FILTER
#elif defined I_AM_CARTE_SUPER	//(I_AM == CARTE_SUPER)
	#define MY_FILTER	SUPER_FILTER
#elif defined I_AM_CARTE_BALISE	//(I_AM == CARTE_BALISE)
	#define MY_FILTER	BALISE_FILTER
#else
	#warning "carte inconnue : définissez I_AM_CARTE_XXXXX : voir QS_can.c"
#endif

#ifndef QS_CAN_RX_IT_PRI
	#define QS_CAN_RX_IT_PRI CAN_INT_PRI_7
#else
	#warning "QS_CAN_RX_IT_PRI redefined"
#endif /* ndef QS_CAN_RX_IT_PRI */

/* Variables globales pour le CAN */
#ifdef USE_CAN
static void CAN_error_processing();
	static CAN_msg_t m_can_buffer[CAN_BUF_SIZE];
	static volatile Uint8 m_can_rx_num;
	static volatile bool_e m_canrx;	// message reçu sur le bus can
	static void CAN_receive(CAN_msg_t* can_msg);
	static direct_treatment_function_pt m_can_direct_treatment_function = NULL;
#endif /* def USE_CAN */
#ifdef USE_CAN2
	static CAN_msg_t m_can2_buffer[CAN_BUF_SIZE];
	static volatile Uint8 m_can2_rx_num;
	static volatile bool_e m_can2rx;	// message reçu sur le bus can2
	static void CAN2_receive(CAN_msg_t* can_msg);
	static direct_treatment_function_pt m_can2_direct_treatment_function_pt = NULL;
#endif /* def USE_CAN2 */

#if defined (USE_CAN) || defined (USE_CAN2)

	void CAN_reinit(void)
	{
		Sint8 i;
		Uint16 CANModeOperating, CANModeConfig, CANInitialize, CANSettings, CANConfigInt, CANIntPriority;

		// vérifié
		CANModeConfig =	CAN_IDLE_CON &
						CAN_MASTERCLOCK_1 & //FCAN clock is Fcy
						CAN_REQ_OPERMODE_CONFIG &
						CAN_CAPTURE_DIS;
		
		//Pas testé à 20 et 40 MHz
		CANInitialize =	CAN_SYNC_JUMP_WIDTH1 &
		#ifdef FREQ_40MHZ
						CAN_BAUD_PRE_SCALE(11);
		#elif defined(FREQ_20MHZ)
						CAN_BAUD_PRE_SCALE(5);
		#else
						CAN_BAUD_PRE_SCALE(2);
			#ifdef FREQ_INTERNAL_CLK
				#warning CAN non fonctionnel sur oscillateur interne
			#endif
		#endif
		
		//ok, sans discuter le choix des valeurs 3/3/3
		CANSettings =	CAN_WAKEUP_BY_FILTER_DIS &
						CAN_PHASE_SEG2_TQ(3) &
						CAN_PHASE_SEG1_TQ(3) &
						CAN_PROPAGATIONTIME_SEG_TQ(3) &
						CAN_SEG2_FREE_PROG &
						CAN_SAMPLE3TIMES;

		//vérifié
		CANConfigInt =	
			#ifdef TEST_MODE
						CAN_INDI_INVMESS_EN &
						CAN_INDI_WAK_EN &
						CAN_INDI_ERR_EN &
						CAN_INDI_TXB2_DIS &
						CAN_INDI_TXB1_DIS &
						CAN_INDI_TXB0_DIS &
			#else
						CAN_INDI_INVMESS_DIS &
						CAN_INDI_WAK_DIS &
						CAN_INDI_ERR_DIS &
						CAN_INDI_TXB2_DIS &
						CAN_INDI_TXB1_DIS &
						CAN_INDI_TXB0_DIS &
			#endif
						CAN_INDI_RXB1_EN &
						CAN_INDI_RXB0_EN;
		
		//vérifié
		CANIntPriority = QS_CAN_RX_IT_PRI
						 & CAN_INT_ENABLE;
						
		//vérifié
		CANModeOperating =	CAN_IDLE_CON &
							CAN_MASTERCLOCK_1 &
							CAN_REQ_OPERMODE_NOR &
							CAN_CAPTURE_DIS;	
		#ifdef USE_CAN
			C1INTF=0;
			IFS1bits.C1IF=0;
			CAN1SetOperationMode(CANModeConfig);
			while(C1CTRLbits.OPMODE <=3);
			
			CAN1Initialize(CANInitialize, CANSettings);
			ConfigIntCAN1(CANConfigInt, CANIntPriority);
		
			C1INTF=0;
			IFS1bits.C1IF=0;

			/*************************************************************************
			* Function Name     : CAN1SetMask
			* Description       : This function sets the values for the acceptance 
			*                     filter mask registers (SID and EID)
			* Parameters        : char: mask_no
			*                     unsigned int: sid register value  
			*                     unsigned long: eid registers value
			* Return Value      : None
			**************************************************************************/
			for(i=0; i<=1; i++)
			#ifndef I_AM_CARTE_SUPER
				CAN1SetMask(i, MASK_BITS<<2, 0);
			#else
				CAN1SetMask(i, 0<<2, 0);
			#endif /* ndef I_AM_CARTE_SUPER */

			/*********************************************************************
			* Function Name     : CAN1SetFilter
			* Description       : This function sets the acceptance filter values 
			*                     (SID and EID) for the specified filter
			* Parameters        : char: filter_no
			*                     unsigned int: sid register value  
			*                     unsigned long: eid registers value
			* Return Value      : None 
			*********************************************************************/
			/* ecrit le sid directement dans C1RXF0SID*/
			for(i=0; i<=5; i++)
			{
				switch(i)
				{
					case 0: 
						CAN1SetFilter(i, MY_FILTER << 2, 0);
						break;
					case 2:
						#ifdef DISABLE_BROADCAST_FILTER	//Si l'on ne veut pas recevoir les messages en broadcast...
							CAN1SetFilter(i, MY_FILTER << 2, 0);
						#else
							CAN1SetFilter(i, BROADCAST_FILTER << 2, 0);
						#endif
						break;
					default:
						CAN1SetFilter(i, 0x7FF << 2, 0);		//Messages de débug recu par toutes les cartes...
						break;	
				}
			}	

			/****************************************************************************
			* Function Name     : CAN1SetRXMode
			* Description       : This function configures the CxRXxCON register
			* Parameters        : char buffno, unsigned int config
			* Return Value      : None 
			*****************************************************************************/
			for(i=0; i<2; i++)//config de tous les registres de reception
				CAN1SetRXMode(i,CAN_RXFUL_CLEAR & CAN_BUF0_DBLBUFFER_DIS);
			

			/****************************************************************************
			* Function Name     : CAN1SetTXMode
			* Description       : This function configures the CxTXxCON register
			* Parameters        : char buffno, unsigned int config
			* Return Value      : None 
			*****************************************************************************/
			for(i=0; i<3; i++)//config de tous les registres d'envoi
				CAN1SetTXMode(i,CAN_TX_STOP_REQ & CAN_TX_PRIORITY_HIGH );

			CAN1SetOperationMode(CANModeOperating);
			#ifndef MODE_SIMULATION
				while(C1CTRLbits.OPMODE !=0);
			#endif
		#endif /* def USE_CAN */

		#ifdef USE_CAN2
		//TODO : recopier CAN1
			C2INTF=0;
			IFS1bits.C2IF=0;
			CAN2SetOperationMode(CANModeConfig);
			CAN2Initialize(CANInitialize, CANSettings);
			ConfigIntCAN2(CANConfigInt, CANIntPriority);
		
			C2INTF=0;
			IFS1bits.C2IF=0;
		
			CAN2SetMask(0, CAN_MASK_SID(MASK_CAN_S) & CAN_MATCH_FILTER_TYPE, CAN_MASK_EID(3));
		
			CAN2SetRXMode(0,CAN_RXFUL_CLEAR & CAN_BUF0_DBLBUFFER_EN);
			CAN2SetRXMode(1,CAN_RXFUL_CLEAR & CAN_BUF0_DBLBUFFER_EN);
			CAN2SetTXMode(0,CAN_TX_STOP_REQ & CAN_TX_PRIORITY_HIGH );
			CAN2SetTXMode(1,CAN_TX_STOP_REQ & CAN_TX_PRIORITY_HIGH );
			CAN2SetTXMode(2,CAN_TX_STOP_REQ & CAN_TX_PRIORITY_HIGH );

			CAN2SetOperationMode(CANModeOperating);
		#endif /* def USE_CAN2 */
	}
	
	void CAN_init(void)
	{
		static bool_e initialized = FALSE;
		if(initialized)
			return;
		initialized = TRUE;

		#ifdef USE_CAN
			/* initialisation du buffer de reception des messages CAN */
			m_can_rx_num = 0;
			m_canrx = 0;
		#endif /* def USE_CAN */

		#ifdef USE_CAN2
			/* initialisation du buffer de reception des messages CAN */
			m_can2_rx_num = 0;
			m_can2rx = 0;
		#endif /* def USE_CAN2 */
		CAN_reinit();
	}
	
#ifdef USE_CAN

	bool_e CAN_data_ready()
	{
		return m_canrx;
	}	
	#ifdef MODE_SIMULATION
		void CAN_send(CAN_msg_t* can_msg)
		{
			//Nothing...
		}	
	#else
		#ifdef CAN_SEND_TIMEOUT_ENABLE
		#define TIMEOUT_CAN_SEND 4196	//10ms de timeout sur la disponibilité du périph CAN...
			void CAN_send(CAN_msg_t* can_msg)
			{
				Uint16 time;
				time = TIMEOUT_CAN_SEND;
				while(!CAN1IsTXReady(0))
				{
					time--;
					if(!time)
					{
						#ifdef LED_ERROR
							LED_ERROR = 1;
						#endif
						return;
					}
				}
				if(can_msg->size > 8)
					can_msg->size = 0;
				CAN1SendMessage((CAN_TX_SID(can_msg->sid)) & (CAN_TX_EID_DIS) & (CAN_SUB_NOR_TX_REQ),
								(CAN_TX_EID(12344)) & (CAN_NOR_TX_REQ),can_msg->data, can_msg->size, 0);
								
				//Solution alternative non tetée :	
				/*	Sint8 used_buffer;
				for(used_buffer=0; !CAN1IsTXReady(used_buffer); used_buffer=(used_buffer+1)%3);
				//void CAN1SendMessage(unsigned int sid, unsigned long eid, unsigned char * data, unsigned char  datalen, char MsgFlag) __attribute__ ((section (".libperi")));
				CAN1SendMessage((CAN_TX_SID(can_msg->sid)) & (CAN_TX_EID_DIS) & (CAN_SUB_NOR_TX_REQ),
							(CAN_TX_EID(12344)) & (CAN_NOR_TX_REQ),can_msg->data, can_msg->size, used_buffer);
				*/
			}
		#else	//Rétrocompatibilité...
			void CAN_send(CAN_msg_t* can_msg)
			{
				while(!CAN1IsTXReady(0));
				if(can_msg->size > 8)	//sécurité si l'utilisateur fait des betises.
					can_msg->size = 0;
				CAN1SendMessage((CAN_TX_SID(can_msg->sid)) & (CAN_TX_EID_DIS) & (CAN_SUB_NOR_TX_REQ),
								(CAN_TX_EID(12344)) & (CAN_NOR_TX_REQ),can_msg->data, can_msg->size, 0);
			}

			/* envoie un message CAN sans données avec le sid spécifié */
			void CAN_send_sid(Uint11 sid)
			{
				CAN_msg_t msg;
				msg.sid = sid;
				msg.size = 0;
				CAN_send(&msg);
			}
		
		#endif
	#endif
	//verifiée
	static void CAN_receive(CAN_msg_t* can_msg)
	{
		if(C1RX0CONbits.RXFUL==1)
		{
			/* réception d'un message dans le buffer RX0 */
			can_msg->sid=C1RX0SID>>2;
			can_msg->size=C1RX0DLCbits.DLC;
			CAN1ReceiveMessage(can_msg->data,can_msg->size,0);
			C1RX0CONbits.RXFUL=0;
			C1INTF=0;
		}
		else if(C1RX1CONbits.RXFUL==1)
		{
			/* réception d'un message dans le buffer RX1 */
			can_msg->sid=C1RX1SID>>2;
			can_msg->size=C1RX1DLCbits.DLC;
			CAN1ReceiveMessage(can_msg->data,can_msg->size,1);
			C1RX1CONbits.RXFUL=0;
			C1INTF=0;
		}
		else 
		{
			can_msg->sid=0;
			can_msg->size=0;
		}
		return;
	}

	//verifiée
	CAN_msg_t CAN_get_next_msg()
	{
		static Uint8 next_to_read =0;
		CAN_msg_t* msgToReturn;
		if (next_to_read != m_can_rx_num)
		{
			#ifdef USE_CAN
				DisableIntCAN1;
			#endif
			// récupérer le prochain message à lire
			msgToReturn = (m_can_buffer + (next_to_read++));
			next_to_read %= CAN_BUF_SIZE;

			// si on lit le dernier message, abaisser le drapeau.
			if (m_can_rx_num == next_to_read)
				m_canrx = FALSE;
			#ifdef USE_CAN
				EnableIntCAN1;
			#endif
			return *msgToReturn;
		}
		else
		{
			//assure que le flag est baissé
			m_canrx=FALSE;
			// renvoie d'un message invalide.
			return (CAN_msg_t) {0, "\0\0\0\0\0\0\0", 0};
		}	
	}
	
	/* Permet le traitement direct des messages CAN */
	void CAN_set_direct_treatment_function(direct_treatment_function_pt fct)
	{
		m_can_direct_treatment_function = fct;
	}
	
	//verifiée
	static void CAN_error_processing()
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
		CAN_reinit();
		debug_printf("End Error List\r\n");
	}

	//verifiée
	void _ISR _C1Interrupt(void)
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
		
		switch( C1CTRLbits.ICODE )
		{
			case RXB0_INTERRUPT:
			case RXB1_INTERRUPT:
				CAN_receive(m_can_buffer + m_can_rx_num);
				if(m_can_direct_treatment_function == NULL || m_can_direct_treatment_function(m_can_buffer + m_can_rx_num))
				{
					m_can_rx_num++;
					m_can_rx_num%=CAN_BUF_SIZE;		
					m_canrx=TRUE;
				}
				C1INTFbits.RX0IF = 0; // Add code to read buffer 0

				C1INTFbits.RX1IF = 0; // Add code to read buffer 1

				break;
			case ERROR_INTERRUPT:
				CAN_error_processing();
				C1INTFbits.ERRIF = 0;
				break;
			default:
				debug_printf("default interrupt %d\n",C1CTRLbits.ICODE);
				C1INTFbits.TX0IF = 0;
				C1INTFbits.TX1IF = 0;
				C1INTFbits.TX2IF = 0;
				C1INTFbits.WAKIF = 0;
				break;
		}
	}

#endif /* def USE_CAN */

#ifdef USE_CAN2
	//TODO : recopier CAN1

	bool_e CAN2_data_ready()
	{
		return m_can2rx;
	}	

	void CAN2_send(CAN_msg_t* can_msg)
	{
		while(!CAN2IsTXReady(0));	
		//void CAN2SendMessage(unsigned int sid, unsigned long eid, unsigned char * data, unsigned char  datalen, char MsgFlag) __attribute__ ((section (".libperi")));

		CAN2SendMessage((CAN_TX_SID(can_msg->sid)) & (CAN_TX_EID_DIS) & (CAN_SUB_NOR_TX_REQ),
						(CAN_TX_EID(12344)) & (CAN_NOR_TX_REQ),can_msg->data, can_msg->size, 0);

	}


	static void CAN2_receive(CAN_msg_t* can_msg)
	{
		if(C2RX0CONbits.RXFUL==1)
		{
			/* réception d'un message dans le buffer RX0 */
			can_msg->sid=C2RX0SID>>2;
			can_msg->size=C2RX0DLCbits.DLC;
			CAN2ReceiveMessage(can_msg->data,can_msg->size,0);
			C2RX0CONbits.RXFUL=0;
			C2INTF=0;
		}
		else if(C2RX1CONbits.RXFUL==1)
		{
			/* réception d'un message dans le buffer RX1 */
			can_msg->sid=C2RX1SID>>2;
			can_msg->size=C2RX1DLCbits.DLC;
			CAN2ReceiveMessage(can_msg->data,can_msg->size,1);
			C2RX1CONbits.RXFUL=0;
			C2INTF=0;
		}
		else 
		{
			can_msg->sid=0;
			can_msg->size=0;
		}
		return;
	}

	CAN_msg_t CAN2_get_next_msg()
	{
		static Uint8 next_to_read =0;
		CAN_msg_t* msgToReturn;
		if (next_to_read != m_can2_rx_num)
		{
			// récupérer le prochain message à lire
			msgToReturn = (m_can2_buffer + (next_to_read++));
			next_to_read %= CAN_BUF_SIZE;

			// si on lit le dernier message, abaisser le drapeau.
			if (m_can2_rx_num == next_to_read)
				m_can2rx = FALSE;

			return *msgToReturn;
		}
		else
		{
			//assure que le flag est baissé
			m_can2rx=FALSE;
			// renvoie d'un message invalide.
			return (CAN_msg_t) {0, "\0\0\0\0\0\0\0", 0};
		}	
	}
	
	/* Permet le traitement direct des messages CAN */
	void CAN2_set_direct_treatment_function(direct_treatment_function_pt fct)
	{
		m_can2_direct_treatment_function = fct;
	}
	
	void _ISR _C2Interrupt(void)
	{		
		CAN2_receive(m_can2_buffer + m_can2_rx_num);	
		if(m_can2_direct_treatment_function == NULL || m_can2_direct_treatment_function(m_can2_buffer + m_can2_rx_num))
		{
				m_can2_rx_num++;
				m_can2_rx_num%=CAN_BUF_SIZE;		
				m_can2rx=TRUE;
		}
		IFS1bits.C2IF=0;
	}
#endif /* def USE_CAN2 */
#endif /* defined (USE_CAN) || defined (USE_CAN2) */
