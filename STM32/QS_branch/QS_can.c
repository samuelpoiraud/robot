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
#include "QS_CANmsgList.h"
#include "QS_ports.h"
#include "QS_outputlog.h"
#include "../stm32f4xx_hal/stm32f4xx_hal_can.h"
#include "../config/config_pin.h"


#define MASK_CAN_S			0x000 /* 0b00000000000 */
/* on filtre les messages nous meme en aval */

#if defined I_AM_CARTE_PROP
	#define MY_FILTER	PROP_FILTER
#elif defined I_AM_CARTE_ACT
	#define MY_FILTER	ACT_FILTER
#elif defined I_AM_CARTE_STRAT
	#define MY_FILTER	STRAT_FILTER
#elif defined I_AM_CARTE_BALISE
	#define MY_FILTER	BALISE_FILTER
#elif defined I_AM_CARTE_IHM
	#define MY_FILTER	IHM_FILTER
#elif defined I_AM_CARTE_BEACON_EYE	//(I_AM == I_AM_CARTE_BEACON_EYE)

#else
	#warning "carte inconnue : définissez I_AM_CARTE_XXXXX : voir QS_can.c"
#endif

#ifndef QS_CAN_RX_IT_PRI
	#define QS_CAN_RX_IT_PRI 1
#else
	#warning "QS_CAN_RX_IT_PRI redefined"
#endif /* ndef QS_CAN_RX_IT_PRI */

/* Variables globales pour le CAN */
#ifdef USE_CAN
	static CAN_HandleTypeDef CAN_HandleStructure;
	static CAN_msg_t m_can_buffer[CAN_BUF_SIZE];
	static volatile Uint8 m_can_rx_num;
	static volatile bool_e m_canrx;	// message reçu sur le bus can
	static void CAN_receive(CAN_msg_t* can_msg);
	static direct_treatment_function_pt m_can_direct_treatment_function = NULL;


	void CAN_reinit(void)
	{
		CAN_FilterConfTypeDef CAN_FilterConfStructure;

		// Initialisation du CAN
		CAN_HandleStructure.Instance = CAN1;
		CAN_HandleStructure.Init.Prescaler = PCLK1_FREQUENCY_HZ / 1000000 / 2;	//Prescaler = 21 -> 1Mhz clk CAN //Pourquoi le 2, je ne sais pas, mais on tombe sur la même vitesse que les dsPIC30F...
		CAN_HandleStructure.Init.Mode = CAN_MODE_NORMAL;
		CAN_HandleStructure.Init.SJW = CAN_SJW_1TQ;
		CAN_HandleStructure.Init.BS1 = CAN_BS1_4TQ;     //1tq propagation + 3tq segment 1
		CAN_HandleStructure.Init.BS2 = CAN_BS2_3TQ;
		CAN_HandleStructure.Init.TTCM = DISABLE;
		CAN_HandleStructure.Init.ABOM = ENABLE;
		CAN_HandleStructure.Init.AWUM = DISABLE;
		CAN_HandleStructure.Init.NART = DISABLE;
		CAN_HandleStructure.Init.RFLM = DISABLE;
		CAN_HandleStructure.Init.TXFP = ENABLE;

		HAL_CAN_DeInit(&CAN_HandleStructure);

		__HAL_RCC_CAN1_CLK_ENABLE();
		PORTS_can_init();

		HAL_CAN_Init(&CAN_HandleStructure);

		//Message pour la carte & message de debug sur FIFO 0
		CAN_FilterConfStructure.FilterNumber = 0;
		CAN_FilterConfStructure.FilterMode = CAN_FILTERMODE_IDMASK;
		CAN_FilterConfStructure.FilterScale = CAN_FILTERSCALE_16BIT;
		CAN_FilterConfStructure.FilterIdHigh = MY_FILTER << 5;
		CAN_FilterConfStructure.FilterIdLow  = 0x0700 << 5;
		CAN_FilterConfStructure.FilterMaskIdHigh = MASK_BITS << 5;
		CAN_FilterConfStructure.FilterMaskIdLow  = MASK_BITS << 5;
		CAN_FilterConfStructure.FilterFIFOAssignment = CAN_FILTER_FIFO0;
		CAN_FilterConfStructure.FilterActivation = ENABLE;
		CAN_FilterConfStructure.BankNumber = 0;
		HAL_CAN_ConfigFilter(&CAN_HandleStructure, &CAN_FilterConfStructure);


		//Message en broadcast (qui peuvent flood) sur FIFO 1
		CAN_FilterConfStructure.FilterNumber = 1;
		CAN_FilterConfStructure.FilterMode = CAN_FILTERMODE_IDMASK;
		CAN_FilterConfStructure.FilterScale = CAN_FILTERSCALE_16BIT;
		CAN_FilterConfStructure.FilterIdHigh = BROADCAST_FILTER << 5;
		CAN_FilterConfStructure.FilterIdLow  = BROADCAST_FILTER << 5;
		CAN_FilterConfStructure.FilterMaskIdHigh = MASK_BITS << 5;
		CAN_FilterConfStructure.FilterMaskIdLow  = MASK_BITS << 5;
		CAN_FilterConfStructure.FilterFIFOAssignment = CAN_FILTER_FIFO1;
		CAN_FilterConfStructure.FilterActivation = ENABLE;
		CAN_FilterConfStructure.BankNumber = 0;
		HAL_CAN_ConfigFilter(&CAN_HandleStructure, &CAN_FilterConfStructure);


		#ifdef XBEE_SIMULATION
			//Message en broadcast (qui peuvent flood) sur FIFO 2
			CAN_FilterConfStructure.FilterNumber = 2;
			CAN_FilterConfStructure.FilterMode = CAN_FILTERMODE_IDMASK;
			CAN_FilterConfStructure.FilterScale = CAN_FILTERSCALE_16BIT;
			CAN_FilterConfStructure.FilterIdHigh = XBEE_FILTER << 5;
			CAN_FilterConfStructure.FilterIdLow  = 0x0700 << 5;
			CAN_FilterConfStructure.FilterMaskIdHigh = MASK_BITS << 5;
			CAN_FilterConfStructure.FilterMaskIdLow  = MASK_BITS << 5;
			CAN_FilterConfStructure.FilterFIFOAssignment = CAN_FILTER_FIFO1;
			CAN_FilterConfStructure.FilterActivation = ENABLE;
			CAN_FilterConfStructure.BankNumber = 0;
			HAL_CAN_ConfigFilter(&CAN_HandleStructure, &CAN_FilterConfStructure);
		#endif

		__HAL_CAN_ENABLE_IT(&CAN_HandleStructure, CAN_IT_FMP0);
		__HAL_CAN_ENABLE_IT(&CAN_HandleStructure, CAN_IT_FMP1);
		__HAL_CAN_ENABLE_IT(&CAN_HandleStructure, CAN_IT_ERR);
		__HAL_CAN_ENABLE_IT(&CAN_HandleStructure, CAN_IT_FOV0); //Recv buffer 0 overrun
		__HAL_CAN_ENABLE_IT(&CAN_HandleStructure, CAN_IT_FOV1); //Recv buffer 1 overrun
		__HAL_CAN_ENABLE_IT(&CAN_HandleStructure, CAN_IT_BOF);  //Bus OFF, TEC / REC > 255

		HAL_NVIC_SetPriority(CAN1_RX0_IRQn, QS_CAN_RX_IT_PRI, 0);
		HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);

		HAL_NVIC_SetPriority(CAN1_RX1_IRQn, QS_CAN_RX_IT_PRI, 0);
		HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);

		HAL_NVIC_SetPriority(CAN1_SCE_IRQn, QS_CAN_RX_IT_PRI, 0);
		HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
	}

	static bool_e initialized = FALSE;

	void CAN_reset(void){
		__CAN1_CLK_DISABLE();
		initialized = FALSE;
	}

	void CAN_init(void)
	{
		if(initialized)
			return;
		initialized = TRUE;

		/* initialisation du buffer de reception des messages CAN */
		m_can_rx_num = 0;
		m_canrx = 0;

		CAN_reinit();
	}


	volatile static CAN_callback_action_t CAN_send_callback = NULL;
	bool_e CAN_data_ready()
	{
		return m_canrx;
	}
	//Enregistre un pointeur sur fonction qui sera appelé à chaque message CAN envoyé.
	void CAN_set_send_callback(CAN_callback_action_t action)
	{
		CAN_send_callback = action;
	}


	#ifdef MODE_SIMULATION
			//EN mode simulation OU si pas de bus CAN utilisé.
			void CAN_send(CAN_msg_t* can_msg)
			{
				if(CAN_send_callback)
					(*CAN_send_callback)(can_msg);
			}
	#else
			//Sans mode simulation, ET USE_CAN


			#ifndef TIMEOUT_CAN_SEND
				#define TIMEOUT_CAN_SEND 4196	//10ms de timeout sur la disponibilité du périph CAN...
			#endif
			void CAN_send(CAN_msg_t* can_msg)
			{
				CanTxMsgTypeDef TxMsg;
				int i;

				#ifdef CAN_SEND_TIMEOUT_ENABLE
					Uint32 time = TIMEOUT_CAN_SEND;
				#endif

				if(can_msg->size > 8)	//sécurité si l'utilisateur fait des betises.
					can_msg->size = 0;

				TxMsg.ExtId = 12344;
				TxMsg.IDE = CAN_ID_STD;
				TxMsg.RTR = CAN_RTR_DATA;

				TxMsg.StdId = can_msg->sid;

				//Copie rapide 4 octet d'un coup
	//			((Uint32*)TxMsg.Data)[0] = ((Uint32*)can_msg->data)[0];
	//			((Uint32*)TxMsg.Data)[1] = ((Uint32*)can_msg->data)[1];

				for(i = 0; i < 8; i++) {
					TxMsg.Data[i] = can_msg->data.raw_data[i];
				}

				TxMsg.DLC = can_msg->size;

				CAN_HandleStructure.pTxMsg = &TxMsg; //Copie du message dans la structure du handle

				while(HAL_CAN_Transmit_IT(&CAN_HandleStructure) == HAL_BUSY) {
					#ifdef CAN_SEND_TIMEOUT_ENABLE
					time--;
					if(time == 0) {
						#ifdef LED_ERROR
							HAL_GPIO_WritePin((LED_ERROR, GPIO_PIN_SET);
						#endif

						return;
					}
					#endif
				}
				if(CAN_send_callback)
					(*CAN_send_callback)(can_msg);

			}
		#endif

		/* envoie un message CAN sans données avec le sid spécifié */
		void CAN_send_sid(Uint11 sid)
		{
			CAN_msg_t msg;
			msg.sid = sid;
			msg.size = 0;
			CAN_send(&msg);
		}

		static void CAN_receive(CAN_msg_t* can_msg)
		{
			int i;

			if(__HAL_CAN_GET_FLAG(&CAN_HandleStructure, CAN_FLAG_FMP0))
			{
				/* réception d'un message dans le buffer RX0 */
				HAL_CAN_Receive_IT(&CAN_HandleStructure, 0);
			}
			else if(__HAL_CAN_GET_FLAG(&CAN_HandleStructure, CAN_FLAG_FMP1))
			{
				/* réception d'un message dans le buffer RX1 */
				HAL_CAN_Receive_IT(&CAN_HandleStructure, 1);
			}
			else //Rien a été reçu ...
			{
				can_msg->sid=0;
				can_msg->size=0;
				return;
			}

			can_msg->sid = CAN_HandleStructure.pRxMsg->StdId;
			can_msg->size = CAN_HandleStructure.pRxMsg->DLC;

			for(i = 0; i < 8; i++) {
				can_msg->data.raw_data[i] = CAN_HandleStructure.pRxMsg->Data[i];
			}
		}

		CAN_msg_t CAN_get_next_msg()
		{
			static Uint8 next_to_read =0;
			CAN_msg_t* msgToReturn;
			if (next_to_read != m_can_rx_num)
			{
				NVIC_DisableIRQ(CAN1_RX0_IRQn);
				NVIC_DisableIRQ(CAN1_RX1_IRQn);

				// récupérer le prochain message à lire
				msgToReturn = &(m_can_buffer[next_to_read++]);
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
				//assure que le flag est baissé
				m_canrx=FALSE;
				// renvoie d'un message invalide.
				return (CAN_msg_t) {0};
			}
		}

		/* Permet le traitement direct des messages CAN */
		void CAN_set_direct_treatment_function(direct_treatment_function_pt fct)
		{
			m_can_direct_treatment_function = fct;
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
			CAN_onMsgReceived();
		}

		void CAN1_RX1_IRQHandler(void)
		{
			CAN_onMsgReceived();
		}

		void CAN1_SCE_IRQHandler(void)
		{
			it_printf("CAN Error List :\n");
			if(__HAL_CAN_GET_FLAG(&CAN_HandleStructure, CAN_FLAG_BOF))
			{
				it_printf("  Bus OFF, too many bus errors, > 255\n");
				__HAL_CAN_DISABLE_IT(&CAN_HandleStructure, CAN_IT_BOF);
			}
			if(__HAL_CAN_GET_FLAG(&CAN_HandleStructure, CAN_FLAG_EPV))
			{
				it_printf("  Error passive, many bus errors, > 127\n");
				__HAL_CAN_DISABLE_IT(&CAN_HandleStructure, CAN_IT_EPV);
			}
			if(__HAL_CAN_GET_FLAG(&CAN_HandleStructure, CAN_FLAG_EWG))
			{
				it_printf("  Error passive, many bus errors, > 96\n");
				__HAL_CAN_DISABLE_IT(&CAN_HandleStructure, CAN_IT_EWG);
			}
			if(__HAL_CAN_GET_FLAG(&CAN_HandleStructure, CAN_FLAG_FOV0))
			{
				it_printf("  Recv FIFO 0 Overflow (MY_FILTER + DEBUG)\n");
				__HAL_CAN_CLEAR_FLAG(&CAN_HandleStructure, CAN_FLAG_FOV0);
				__HAL_CAN_DISABLE_IT(&CAN_HandleStructure, CAN_IT_FOV0);
			}

			if(__HAL_CAN_GET_FLAG(&CAN_HandleStructure, CAN_FLAG_FOV1))
			{
				it_printf("  Recv FIFO 1 Overflow (BROADCAST msgs)\n");
				__HAL_CAN_CLEAR_FLAG(&CAN_HandleStructure, CAN_FLAG_FOV1);
				__HAL_CAN_DISABLE_IT(&CAN_HandleStructure, CAN_IT_FOV1);
			}


			it_printf("  Last ErrorCode was: ");
			switch(HAL_CAN_GetError(CAN1)) {
				case HAL_CAN_ERROR_NONE: 			it_printf("No error\n"); break;
				case HAL_CAN_ERROR_STF:				it_printf("Stuff error\n"); break;
				case HAL_CAN_ERROR_FOR:				it_printf("Form error\n"); break;
				case HAL_CAN_ERROR_ACK: 			it_printf("Ack error\n"); break;
				case HAL_CAN_ERROR_BR:				it_printf("BitRecessive error\n"); break;
				case HAL_CAN_ERROR_BD:				it_printf("BitDominant error\n"); break;
				case HAL_CAN_ERROR_CRC: 			it_printf("CRC error\n"); break;
				case HAL_CAN_ERROR_EWG:				it_printf("EWG error\n"); break;
				case HAL_CAN_ERROR_EPV:				it_printf("EPV error\n"); break;
				case HAL_CAN_ERROR_BOF:				it_printf("BOF error\n"); break;
				default: 							it_printf("Unknown\n"); break;
			}
			__HAL_CAN_DISABLE_IT(&CAN_HandleStructure, CAN_IT_ERR);
			CAN_reinit();
			it_printf("End Error List\n");
		}
#else /* def USE_CAN */
	//Pas de bus can, définition des fonctions pour que le code compile quand même
	void CAN_init(void) {}
	bool_e CAN_data_ready() { return FALSE; }
	void CAN_set_send_callback(CAN_callback_action_t action) {}
	void CAN_send(CAN_msg_t* can_msg) {}
	void CAN_direct_send(Uint11 sid, Uint8 size, Uint8 arg[]){}
	void CAN_send_sid(Uint11 sid) {}
	CAN_msg_t CAN_get_next_msg() { static CAN_msg_t msg = {0}; return msg; }
	void CAN_set_direct_treatment_function(direct_treatment_function_pt fct) {}
#endif

#ifdef USE_CAN2
	#error "CAN2 is not implemented"
#endif


