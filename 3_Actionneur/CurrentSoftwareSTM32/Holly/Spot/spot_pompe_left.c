/*  Club Robot ESEO 2014 - 2015
 *
 *	Fichier : spot_pompe_left.c
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe pour attraper les spots arrière gauche
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#include "spot_pompe_left.h"

#ifdef I_AM_ROBOT_BIG

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "../selftest.h"

#define LOG_PREFIX "spot_pompe_left : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_SPOT_POMPE_LEFT
#include "../QS/QS_outputlog.h"

static void SPOT_POMPE_LEFT_initDCM();
static void SPOT_POMPE_LEFT_command_init(queue_id_t queueId);
static void SPOT_POMPE_LEFT_command_run(queue_id_t queueId);
static void SPOT_POMPE_LEFT_do_order(Uint8 command, Uint8 param);
static void SPOT_POMPE_LEFT_run_command(queue_id_t queueId, bool_e init);
static void SPOT_POMPE_PWM_run(Uint8 duty /* en pourcents*/);
static void SPOT_POMPE_PWM_stop();

static volatile bool_e PWM_initialized = FALSE;

void SPOT_POMPE_LEFT_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	SPOT_POMPE_LEFT_initDCM();
}

void SPOT_POMPE_LEFT_reset_config(){}

// Initialisation du moteur, si init ne fait rien
static void SPOT_POMPE_LEFT_initDCM() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

#ifdef USE_CUSTOM_PWM_5
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	GPIO_InitTypeDef GPInit;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	GPInit.GPIO_Mode = GPIO_Mode_AF;
	GPInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPInit.GPIO_OType = GPIO_OType_PP;
	GPInit.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPInit.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOE, &GPInit);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_TIM1);

	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period        = PWM_HIGH_SPEED_DUTY - 1;	//Le timer compte de 0 à period inclus
	TIM_TimeBaseStructure.TIM_Prescaler     = (TIM_CLK2_FREQUENCY_HZ / PWM_FREQ / PWM_HIGH_SPEED_DUTY) - 1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	PWM_initialized = TRUE;
#endif

	SPOT_POMPE_PWM_stop();

	info_printf("initialisée (DCM) \n");
}

void SPOT_POMPE_LEFT_stop() {
	SPOT_POMPE_PWM_stop();
}

void SPOT_POMPE_LEFT_init_pos(){
	SPOT_POMPE_PWM_stop();
}

bool_e SPOT_POMPE_LEFT_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_SPOT_POMPE_LEFT) {
		SPOT_POMPE_LEFT_initDCM();
		switch(msg->data.act_msg.order) {
			case ACT_SPOT_POMPE_LEFT_NORMAL:
			case ACT_SPOT_POMPE_LEFT_REVERSE:
			case ACT_SPOT_POMPE_LEFT_STOP:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_SPOT_POMPE_LEFT, &SPOT_POMPE_LEFT_run_command, msg->data.act_msg.act_data.act_optionnal_data[0], TRUE);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST) {
		SELFTEST_set_actions(&SPOT_POMPE_LEFT_run_command, 27, 3, (SELFTEST_action_t[]){
								 {ACT_SPOT_POMPE_LEFT_NORMAL,  100, QUEUE_ACT_SPOT_POMPE_LEFT},
								 {ACT_SPOT_POMPE_LEFT_REVERSE, 100, QUEUE_ACT_SPOT_POMPE_LEFT},
								 {ACT_SPOT_POMPE_LEFT_STOP, 0, QUEUE_ACT_SPOT_POMPE_LEFT}
							 });
	}

	return FALSE;
}

static void SPOT_POMPE_LEFT_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_SPOT_POMPE_LEFT) {
			if(init)
				SPOT_POMPE_LEFT_command_init(queueId);
			else
				SPOT_POMPE_LEFT_command_run(queueId);
	}
}

//Initialise une commande
static void SPOT_POMPE_LEFT_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint8 param = QUEUE_get_arg(queueId)->param;

	switch(command) {
		case ACT_SPOT_POMPE_LEFT_NORMAL:
		case ACT_SPOT_POMPE_LEFT_REVERSE:
			SPOT_POMPE_LEFT_do_order(command, param);
			break;

		case ACT_SPOT_POMPE_LEFT_STOP:
			SPOT_POMPE_PWM_stop();
			return;

		default: {
			error_printf("Invalid SPOT_POMPE_LEFT command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_SPOT_POMPE_LEFT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}
}

static void SPOT_POMPE_LEFT_command_run(queue_id_t queueId){
	QUEUE_next(queueId, ACT_SPOT_POMPE_LEFT, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0);
}

static void SPOT_POMPE_LEFT_do_order(Uint8 command, Uint8 param){
	if(command == ACT_SPOT_POMPE_LEFT_NORMAL)
		GPIO_ResetBits(SPOT_POMPE_LEFT_SENS);
	else if(command == ACT_SPOT_POMPE_LEFT_REVERSE)
		GPIO_SetBits(SPOT_POMPE_LEFT_SENS);
	else{
		debug_printf("commande envoyée à SPOT_POMPE_LEFT_do_order inconnue -> %d	%x\n", command, command);
		SPOT_POMPE_PWM_stop();
		return;
	}

	param = (param > 100) ? 100 : param;
	SPOT_POMPE_PWM_run(param);
}

static void SPOT_POMPE_PWM_run(Uint8 duty /* en pourcents*/)
{
	if(!PWM_initialized)
		return;
	TIM_SetCompare4(TIM1, duty * (PWM_HIGH_SPEED_DUTY/100));
}

static void SPOT_POMPE_PWM_stop()
{
	SPOT_POMPE_PWM_run(0);
}

#endif
