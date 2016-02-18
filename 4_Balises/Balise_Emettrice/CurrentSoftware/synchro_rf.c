/*
 *	Club Robot ESEO 2012-2015
 *
 *	Fichier : motor.c
 *	Package : Balise émettrice
 *	Description : Gestion du pilotage du moteur avec asservissement en vitesse.
 *					La vitesse angulaire doit être le plus constante possible.
 *	Auteur : Nirgal(PIC)/Arnaud(STM32)
 *	Version 201203
 */

#include "synchro_rf.h"

#include "QS/QS_rf.h"
#include "QS/QS_timer.h"
#include "QS/QS_outputlog.h"
#include "QS/impl/QS_uart_impl.h"
#include "motor.h"
#include "emission_ir.h"

#define TIME_BASE_UNIT		100		//en [us]
#define DUREE_STEP			2000	//Durée d'un step [us]

#define FLASH_CYCLE			(PERIODE_FLASH*NOMBRE_BALISES_EMETTRICES)	//Un cycle d'émission des N balises

#define TIME_PER_MODULE		50		//en [step]
#define TIME_WHEN_SYNCHRO	TIME_PER_MODULE*RF_module_id		//[nombre de step]
#define TOTAL_STEP_COUNT	(RF_MODULE_COUNT*TIME_PER_MODULE)	//[nombre de step] step_ir ? [0; TOTAL_STEP_COUNT[

//Deviation obesrvée entre pic et stm32 après 15min de fonctionnement: 0.1ms/s
//On décide que 20 sec sans synchro c'est trop et qu'il faut repasser en émission continue
#define SYNCHRO_TIMEOUT 10000 //en [nombre de step], 10000 = 20s


#if (TOTAL_STEP_COUNT % FLASH_CYCLE) != 0
	#error "Le temps d'un cycle d'emission doit être un multiple du temps total de la base de temps de synchro rf"
#endif

#if TOTAL_STEP_COUNT > 255
	#error "Il faut utiliser du Uint16 pour la variable step_ir !!!!!! Sinon ça va overflow"
#endif

static volatile bool_e is_synchronized = FALSE;
static volatile bool_e synchro_received;
static volatile Uint16 request_duration;
static volatile Sint16 offset;
static volatile Uint8 failed_synchro_count;
static volatile RF_module_e RF_module_id = RF_FOE1;

//Varie de 0 à 50*nbmodules, 100 ms / module
static volatile Uint8 step_ir = 0;

static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size);
static void SyncRF_next_step(void);

void SYNCRF_init()
{
	is_synchronized = FALSE;
	synchro_received = FALSE;

	request_duration = 0;
	failed_synchro_count = 0;
	offset = 0x0FFF;

	if(QS_WHO_AM_I_get() == BIG_ROBOT)
		RF_module_id = RF_FOE1;
	else
		RF_module_id = RF_FOE2;

	GPIO_SetBits(RF_CONFIG);
	RF_init(RF_module_id, &rf_packet_received_callback, NULL);
	UART_IMPL_write(RF_UART, 'X');
	TIMER3_run_us(1000);	//[1000us = 1ms]
}


void SYNCRF_process_main() {

	if(request_duration > 0) {
		info_printf("Req : %dms\n", request_duration * DUREE_STEP);
		request_duration = 0;
	}

	if(offset != 0x0FFF) {
		info_printf("Offs: %dms\n", offset * DUREE_STEP);
		offset = 0x0FFF;
	}
}


void SyncRF_process_it(){
	SyncRF_next_step();
}





//@pre Appeler cette fonction toutes les 2ms
static void SyncRF_next_step(void)
{
	//On compte de 0 à TOTAL_STEP_COUNT
	step_ir = (step_ir >= TOTAL_STEP_COUNT - 1)? 0: step_ir+1;

	if(step_ir < TOTAL_STEP_COUNT/2 && is_synchronized)
		GPIO_SetBits(LED_G_SYNC);
	else
		GPIO_ResetBits(LED_G_SYNC);

	//Effectue une demande de synchro
	if(step_ir == TIME_WHEN_SYNCHRO)
		SYNCRF_sendRequest();

	//Si on veut le mode double emetteurs et qu'on a une synchro
	if(is_synchronized)
	{
		Uint8 step_in_period = step_ir % FLASH_CYCLE;

		if(QS_WHO_AM_I_get() == BIG_ROBOT){
			if(step_in_period >= NO_FLASH_TIME && step_in_period < PERIODE_FLASH - NO_FLASH_TIME)
				EmissionIR_activate();
			else
				EmissionIR_deactivate();
		}else{
			if(step_in_period >= PERIODE_FLASH + NO_FLASH_TIME && step_in_period < 2*PERIODE_FLASH - NO_FLASH_TIME)
				EmissionIR_activate();
			else
				EmissionIR_deactivate();
		}
	}
	else
		EmissionIR_activate();
	//Le décompte du temps est maintenu même si l'on est en simple émetteurs,
	//pour permettre un passage plus tard (= après la synchro) en mode de double émission !
}




void SYNCRF_sendRequest() {
	RF_synchro_request(RF_BROADCAST);

	//Verif de la synchro précédente (on fait la demande avant, au plus près de l'IT, la réponse reviendra dans assez longtemps (80ms), on a le temps
	if(synchro_received)
	{
		GPIO_ResetBits(LED_R_SYNC);
		failed_synchro_count = 0;
		is_synchronized = TRUE; //Synchro reçue
	}
	else if(failed_synchro_count < (SYNCHRO_TIMEOUT/TOTAL_STEP_COUNT))
	{
		toggle_led(LED_R_SYNC);
		failed_synchro_count++;
	}
	else
	{
		//Si on a fail trop de fois à suivre, on indique qu'on est plus synchro (et donc passage en mode 1 adversaire, on emet des IR tout le temps)
		is_synchronized = FALSE;
		toggle_led(LED_R_SYNC);
	}
	synchro_received = FALSE;
}

//Cette fonction est appelée pour tout paquet RF qui est arrivé (crc vérifié)
static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size) {
	if(for_me && header.type == RF_PT_SynchroResponse)
	{
		if(step_ir > TIME_WHEN_SYNCHRO)
			request_duration = step_ir - TIME_WHEN_SYNCHRO;
		else
			request_duration = step_ir + TOTAL_STEP_COUNT - TIME_WHEN_SYNCHRO;

		//unités: tick / (us/localtick / us/tick) = tick * us/tick / us/localtick = us/(us/localtick) = localtick
		Sint16 fullOffset = (data[0] | data[1] << 8) / (DUREE_STEP / TIME_BASE_UNIT);

		offset = fullOffset - (request_duration / 2) + 2; // fullOffset - request_duration/2 + erreur systématique (en step)
		const Sint16 adjusted = ((Sint16)step_ir) + offset;

		if(adjusted >= TOTAL_STEP_COUNT)
			step_ir = adjusted - TOTAL_STEP_COUNT;
		else if(adjusted >= 0)
			step_ir = adjusted;
		else
			step_ir = adjusted + TOTAL_STEP_COUNT;

		synchro_received = TRUE;
	}
}
