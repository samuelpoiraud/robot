/*
 *	Club Robot ESEO 2016 - 2017
 *	Harry & Anne
 *
 *	Fichier : mosfetBoard.c
 *	Package : Actionneur
 *	Description : Communication avec la carte mosfet
 *  Auteurs : Arnaud
 *  Version 20110225
 */

#include "mosfetBoard.h"
#include "QS/QS_uart.h"
#include "QS/QS_can_over_uart.h"
#include "QS/QS_can_verbose.h"
#include "QS/QS_can.h"

#ifdef USE_MOSTFET_BOARD

	#if MOSFET_BOARD_UART == 1
		#define MOSFET_BOARD_dataReady		UART1_data_ready
		#define MOSFET_BOARD_sendCan		u1rxToCANmsg
		#define MOSFET_BOARD_receiveCan		CANmsgToU1tx
	#elif MOSFET_BOARD_UART == 2
		#define MOSFET_BOARD_dataReady		UART2_data_ready
		#define MOSFET_BOARD_receiveCan		u2rxToCANmsg
		#define MOSFET_BOARD_sendCan		CANmsgToU2tx
		#define MOSFET_BOARD_getByte		UART2_get_next_msg
	#elif MOSFET_BOARD_UART == 6
		#define MOSFET_BOARD_dataReady		UART6_data_ready
		#define MOSFET_BOARD_receiveCan		u6rxToCANmsg
		#define MOSFET_BOARD_sendCan		CANmsgToU6tx
		#define MOSFET_BOARD_getByte		UART6_get_next_msg

	#else
		#ifdef MOSFET_BOARD_UART
			#error "Uart de la carte mosfet non implémenté"
		#else
			#error "MOSFET_BOARD_UART non déclaré"
		#endif
	#endif

	static void MOSFET_BOARD_secretary(CAN_msg_t * msg);

	void MOSFET_BOARD_init(){
		UART_init();
	}

	void MOSFET_BOARD_processMain(){
		while(MOSFET_BOARD_dataReady()){
			CAN_msg_t receivedMsg;
			if(MOSFET_BOARD_receiveCan(&receivedMsg, MOSFET_BOARD_getByte())){
				MOSFET_BOARD_secretary(&receivedMsg);
				#ifdef CAN_VERBOSE_MODE
					QS_CAN_VERBOSE_can_msg_print(&receivedMsg, VERB_INPUT_MSG);
				#endif
			}
		}
	}

	static void MOSFET_BOARD_secretary(CAN_msg_t * msg){
		switch(msg->sid){
			case MOSFET_BOARD_TELL_MOSFET_CURRENT_STATE:{
				CAN_msg_t msg;
				msg.sid = ACT_TELL_MOSFET_CURRENT_STATE;
				msg.size = SIZE_ACT_TELL_MOSFET_CURRENT_STATE;
				msg.data.act_tell_mosfet_state.id = msg.data.mosfet_board_tell_mosfet_state.id;
				msg.data.act_tell_mosfet_state.state = msg.data.mosfet_board_tell_mosfet_state.state;
				CAN_send(&msg);
			}break;

			case MOSFET_BOARD_TELL_TURBINE_SPEED:{
				CAN_msg_t msg;
				msg.sid = ACT_TELL_TURBINE_SPEED;
				msg.size = SIZE_ACT_TELL_TURBINE_SPEED;
				msg.data.act_tell_turbine_speed.speed = msg.data.mosfet_board_tell_turbine_speed.speed;
				CAN_send(&msg);
			}break;

			default:
				break;
		}

	}

	void MOSFET_BOARD_getTurbineSpeed(){
		CAN_msg_t msg;

		msg.sid = MOSFET_BOARD_GET_TURBINE_SPEED;
		msg.size = SIZE_MOSFET_BOARD_GET_TURBINE_SPEED;

		MOSFET_BOARD_sendCan(&msg);
	}

	void MOSFET_BOARD_setTurbineSpeed(Uint16 speed){
		CAN_msg_t msg;

		msg.sid = MOSFET_BOARD_SET_TURBINE_SPEED;
		msg.size = SIZE_MOSFET_BOARD_SET_TURBINE_SPEED;
		msg.data.mosfet_board_set_turbine_speed.speed = speed;

		MOSFET_BOARD_sendCan(&msg);
	}

	void MOSFET_BOARD_getPumpStatus(Uint8 id){
		CAN_msg_t msg;

		msg.sid = MOSFET_BOARD_GET_MOSFET_CURRENT_STATE;
		msg.size = SIZE_MOSFET_BOARD_GET_MOSFET_CURRENT_STATE;
		msg.data.mosfet_board_get_mosfet_state.id = id;

		MOSFET_BOARD_sendCan(&msg);
	}

	void MOSFET_BOARD_setMostfet(Uint8 id, bool_e state){
		CAN_msg_t msg;

		msg.sid = MOSFET_BOARD_SET_MOSFET;
		msg.size = SIZE_MOSFET_BOARD_SET_MOSFET;
		msg.data.mosfet_board_set_mosfet.id = id;
		msg.data.mosfet_board_set_mosfet.state = state;

		MOSFET_BOARD_sendCan(&msg);
	}

	void MOSFET_BOARD_putCanMsg(CAN_msg_t * msg){
		MOSFET_BOARD_sendCan(msg);
	}


	void MOSFET_BOARD_ACT_init(){
		MOSFET_BOARD_init();
	}

	void MOSFET_BOARD_ACT_init_pos(){
		MOSFET_BOARD_ACT_stop();
	}

	void MOSFET_BOARD_ACT_stop(){
		MOSFET_BOARD_setMostfet(0, FALSE);
		MOSFET_BOARD_setMostfet(1, FALSE);
		MOSFET_BOARD_setMostfet(2, FALSE);
		MOSFET_BOARD_setMostfet(3, FALSE);
		MOSFET_BOARD_setMostfet(4, FALSE);
		MOSFET_BOARD_setMostfet(5, FALSE);
		MOSFET_BOARD_setMostfet(6, FALSE);
		MOSFET_BOARD_setMostfet(7, FALSE);
	}

	bool_e MOSFET_BOARD_ACT_CAN_process_msg(CAN_msg_t* msg){

		if(msg->sid >= ACT_MOSFET_1 && msg->sid <= ACT_MOSFET_8){
			Uint8 id = msg->sid - ACT_MOSFET_1;

			if(msg->data.act_msg.order == ACT_MOSFET_STOP){
				MOSFET_BOARD_setMostfet(id, FALSE);
			}else if(msg->data.act_msg.order == ACT_MOSFET_NORMAL){
				MOSFET_BOARD_setMostfet(id, TRUE);
			}else{
				MOSFET_BOARD_setMostfet(id, FALSE);
			}
			return TRUE;
		}

		return FALSE;
	}

	void MOSFET_BOARD_ACT_reset_config(){

	}

#endif
