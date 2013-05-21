/*
 *  Club Robot ESEO 2006 - 2009
 *  Game Hoover - Archi-Tech'
 *
 *  Fichier : QS_can.c
 *  Package : Qualit� Soft
 *  Description : fonction CAN
 *  Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 27032011
 */

#include "QS_can.h"
#include "QS_CANmsgList.h"
#include "QS_ports.h"
#include "stm32f4xx_can.h"

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
	#warning "carte inconnue : d�finissez I_AM_CARTE_XXXXX : voir QS_can.c"
#endif

#ifndef QS_CAN_RX_IT_PRI
	#define QS_CAN_RX_IT_PRI 9
#else
	#warning "QS_CAN_RX_IT_PRI redefined"
#endif /* ndef QS_CAN_RX_IT_PRI */

/* Variables globales pour le CAN */
#ifdef USE_CAN
static void CAN_error_processing();
	static CAN_msg_t m_can_buffer[CAN_BUF_SIZE];
	static volatile Uint8 m_can_rx_num;
	static volatile bool_e m_canrx;	// message re�u sur le bus can
	static void CAN_receive(CAN_msg_t* can_msg);
	static direct_treatment_function_pt m_can_direct_treatment_function = NULL;
#endif /* def USE_CAN */
#ifdef USE_CAN2
	static CAN_msg_t m_can2_buffer[CAN_BUF_SIZE];
	static volatile Uint8 m_can2_rx_num;
	static volatile bool_e m_can2rx;	// message re�u sur le bus can2
	static void CAN2_receive(CAN_msg_t* can_msg);
	static direct_treatment_function_pt m_can2_direct_treatment_function_pt = NULL;
#endif /* def USE_CAN2 */

#if defined (USE_CAN) || defined (USE_CAN2)

	void CAN_reinit(void)
	{
		CAN_InitTypeDef CAN_InitStructure;
		CAN_FilterInitTypeDef CAN_FilterInitStructure;

		PORTS_can_init();

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
		CAN_DeInit(CAN1);

		CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
		CAN_InitStructure.CAN_Prescaler = PCLK1_FREQUENCY_HZ / 1250000;	//1,25Mhz clk CAN
		CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
		CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;	//3tq propagation + 3tq segment 1
		CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
		CAN_InitStructure.CAN_ABOM = ENABLE;
		CAN_InitStructure.CAN_AWUM = DISABLE;
		CAN_InitStructure.CAN_NART = DISABLE;
		CAN_InitStructure.CAN_RFLM = DISABLE;
		CAN_InitStructure.CAN_TTCM = DISABLE;
		CAN_InitStructure.CAN_TXFP = DISABLE;
		CAN_Init(CAN1, &CAN_InitStructure);

		#ifndef I_AM_CARTE_SUPER	//La carte super re�oi tout => pas de filtrage
			//Message pour la carte & message de debug sur FIFO 0
			CAN_FilterInitStructure.CAN_FilterNumber = 0;
			CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
			CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
			CAN_FilterInitStructure.CAN_FilterIdHigh = MY_FILTER;
			CAN_FilterInitStructure.CAN_FilterIdLow  = 0x0700;
			CAN_FilterInitStructure.CAN_FilterMaskIdHigh = MASK_BITS;
			CAN_FilterInitStructure.CAN_FilterMaskIdLow  = MASK_BITS;
			CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
			CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
			CAN_FilterInit(&CAN_FilterInitStructure);

			//Message en broadcast (qui peuvent flood) sur FIFO 1
			CAN_FilterInitStructure.CAN_FilterNumber = 1;
			CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
			CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
			CAN_FilterInitStructure.CAN_FilterIdHigh = BROADCAST_FILTER;
			CAN_FilterInitStructure.CAN_FilterIdLow  = BROADCAST_FILTER;
			CAN_FilterInitStructure.CAN_FilterMaskIdHigh = MASK_BITS;
			CAN_FilterInitStructure.CAN_FilterMaskIdLow  = MASK_BITS;
			CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 1;
			CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
			CAN_FilterInit(&CAN_FilterInitStructure);
		#endif
			
		CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
		CAN_ITConfig(CAN1, CAN_IT_FMP1, ENABLE);
		CAN_ITConfig(CAN1, CAN_IT_ERR, ENABLE);
		CAN_ITConfig(CAN1, CAN_IT_FOV0, ENABLE);	//Recv buffer 0 overrun
		CAN_ITConfig(CAN1, CAN_IT_FOV1, ENABLE);	//Recv buffer 1 overrun
//		CAN_ITConfig(CAN1, CAN_IT_EWG, ENABLE);	//TEC / REC >= 96
//		CAN_ITConfig(CAN1, CAN_IT_EPV, ENABLE);	//TEC / REC > 127
		CAN_ITConfig(CAN1, CAN_IT_BOF, ENABLE);	//Bus OFF, TEC / REC > 255

		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = QS_CAN_RX_IT_PRI;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

		NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
		NVIC_Init(&NVIC_InitStructure);

		NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
		NVIC_Init(&NVIC_InitStructure);

		NVIC_InitStructure.NVIC_IRQChannel = CAN1_SCE_IRQn;
		NVIC_Init(&NVIC_InitStructure);
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
		#define TIMEOUT_CAN_SEND 4196	//10ms de timeout sur la disponibilit� du p�riph CAN...
		void CAN_send(CAN_msg_t* can_msg)
		{
			CanTxMsg TxMsg;

#ifdef CAN_SEND_TIMEOUT_ENABLE
			Uint32 time = TIMEOUT_CAN_SEND;
#endif

			if(can_msg->size > 8)	//s�curit� si l'utilisateur fait des betises.
				can_msg->size = 0;

			TxMsg.ExtId = 12344;
			TxMsg.IDE = CAN_Id_Standard;
			TxMsg.RTR = CAN_RTR_Data;

			TxMsg.StdId = can_msg->sid;

			//Copie rapide 4 octet d'un coup
			((Uint32*)TxMsg.Data)[0] = ((Uint32*)can_msg->data)[0];
			((Uint32*)TxMsg.Data)[1] = ((Uint32*)can_msg->data)[1];

			TxMsg.DLC = can_msg->size;

			while(CAN_Transmit(CAN1, &TxMsg) == CAN_TxStatus_NoMailBox) {
				#ifdef CAN_SEND_TIMEOUT_ENABLE
				time--;
				if(time == 0) {
					#ifdef LED_ERROR
						LED_ERROR = 1;
					#endif
					return;
				}
				#endif
			}
		}

		/* envoie un message CAN sans donn�es avec le sid sp�cifi� */
		void CAN_send_sid(Uint11 sid)
		{
			CAN_msg_t msg;
			msg.sid = sid;
			msg.size = 0;
			CAN_send(&msg);
		}
	#endif

	static void CAN_receive(CAN_msg_t* can_msg)
	{
		CanRxMsg msg;

		if(CAN_GetFlagStatus(CAN1, CAN_FLAG_FMP0))
		{
			/* r�ception d'un message dans le buffer RX0 */
			CAN_Receive(CAN1, 0, &msg);
		}
		else if(CAN_GetFlagStatus(CAN1, CAN_FLAG_FMP1))
		{
			/* r�ception d'un message dans le buffer RX1 */
			CAN_Receive(CAN1, 1, &msg);
		}
		else //Rien a �t� re�u ...
		{
			can_msg->sid=0;
			can_msg->size=0;
			return;
		}

		can_msg->sid = msg.StdId;
		can_msg->size = msg.DLC;

		//Copie rapide 4 octet d'un coup
		((Uint32*)can_msg->data)[0] = ((Uint32*)msg.Data)[0];
		((Uint32*)can_msg->data)[1] = ((Uint32*)msg.Data)[1];
	}

	CAN_msg_t CAN_get_next_msg()
	{
		static Uint8 next_to_read =0;
		CAN_msg_t* msgToReturn;
		if (next_to_read != m_can_rx_num)
		{
			NVIC_DisableIRQ(CAN1_RX0_IRQn);
			NVIC_DisableIRQ(CAN1_RX1_IRQn);

			// r�cup�rer le prochain message � lire
			msgToReturn = (m_can_buffer + (next_to_read++));
			next_to_read %= CAN_BUF_SIZE;

			// si on lit le dernier message, abaisser le drapeau.
			if (m_can_rx_num == next_to_read)
				m_canrx = FALSE;

			NVIC_EnableIRQ(CAN1_RX0_IRQn);
			NVIC_EnableIRQ(CAN1_RX1_IRQn);

			return *msgToReturn;
		}
		else
		{
			//assure que le flag est baiss�
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

	static void CAN_error_processing()
	{
		debug_printf("CAN Error List :\r\n");
		if(CAN_GetFlagStatus(CAN1, CAN_FLAG_BOF))
			debug_printf("  Bus OFF, too many bus errors, > 255\n");
		if(CAN_GetFlagStatus(CAN1, CAN_FLAG_EPV))
			debug_printf("  Error passive, many bus errors, > 127\n");
		if(CAN_GetFlagStatus(CAN1, CAN_FLAG_EWG))
			debug_printf("  Error passive, many bus errors, > 96\n");
		if(CAN_GetFlagStatus(CAN1, CAN_FLAG_FOV0)) {
			debug_printf("  Recv FIFO 0 Overflow (MY_FILTER + DEBUG)\n");

		}
		if(CAN_GetFlagStatus(CAN1, CAN_FLAG_FOV1))
			debug_printf("  Recv FIFO 1 Overflow (BROADCAST msgs)\n");
		debug_printf("  Last ErrorCode was: ");
		switch(CAN_GetLastErrorCode(CAN1)) {
			case CAN_ErrorCode_NoErr: debug_printf("NoErr\n"); break;
			case CAN_ErrorCode_StuffErr: debug_printf("StuffErr\n"); break;
			case CAN_ErrorCode_FormErr: debug_printf("FormErr\n"); break;
			case CAN_ErrorCode_ACKErr: debug_printf("AckErr\n"); break;
			case CAN_ErrorCode_BitRecessiveErr: debug_printf("BitRecessiveErr\n"); break;
			case CAN_ErrorCode_BitDominantErr: debug_printf("BitDominantErr\n"); break;
			case CAN_ErrorCode_CRCErr: debug_printf("CRCErr\n"); break;
			case CAN_ErrorCode_SoftwareSetErr: debug_printf("SoftwareSetErr\n"); break;
			default: debug_printf("Unknown\n"); break;
		}
		debug_printf("  Recv error count: %u\n", CAN_GetReceiveErrorCounter(CAN1));
		debug_printf("  Transmit error count: %u\n", CAN_GetLSBTransmitErrorCounter(CAN1));
		CAN_reinit();
		debug_printf("End Error List\r\n");
	}


	void CAN_onMsgReceived() {
		CAN_receive(&m_can_buffer[m_can_rx_num]);
		if(m_can_direct_treatment_function == NULL || m_can_direct_treatment_function(&m_can_buffer[m_can_rx_num]))
		{
			m_can_rx_num++;
			m_can_rx_num %= CAN_BUF_SIZE;
			m_canrx = TRUE;
		}
	}

	void CAN1_RX0_IRQHandler(void)
	{
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
		CAN_onMsgReceived();
	}

	void CAN1_RX1_IRQHandler(void)
	{
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP1);
		CAN_onMsgReceived();
	}

	void CAN1_SCE_IRQHandler(void)
	{
		CAN_error_processing();

		CAN_ClearITPendingBit(CAN1, CAN_IT_ERR);
		CAN_ClearITPendingBit(CAN1, CAN_IT_FOV0);
		CAN_ClearITPendingBit(CAN1, CAN_IT_FOV1);
	}

#endif /* def USE_CAN */

#ifdef USE_CAN2
	#error "CAN2 is not implemented"
#endif

#endif /* defined (USE_CAN) || defined (USE_CAN2) */
