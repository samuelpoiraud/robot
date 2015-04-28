#include "SynchroRF.h"
#include "EmissionIR.h"

#include "QS/QS_rf.h"
#include "QS/QS_timer.h"
#include "QS/QS_outputlog.h"
#include "QS/impl/QS_uart_impl.h"
#include "motor.h"


#if (TOTAL_STEP_COUNT % FLASH_CYCLE) != 0
#error "Le temps d'un cycle d'emission doit être un multiple du temps total de la base de temps de synchro rf"
#endif

#if TOTAL_STEP_COUNT > 255
#error "Il faut utiliser du Uint16 pour la variable step_ir !!!!!! Sinon ça va overflow"
#endif

static volatile bool_e is_synchronized = FALSE;
static bool_e synchro_received;
static Uint16 request_duration;
static Sint16 offset;
static Uint8 failed_synchro_count;

static bool_e canmsg_received;
static CAN_msg_t canmsg_pending;

static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size);
static void rf_can_received_callback(CAN_msg_t *msg);
static void SyncRF_next_step(void);

void SYNCRF_init()
{
	is_synchronized = FALSE;

	synchro_received = FALSE;
	request_duration = 0;
	offset = 0x0FFF;
	failed_synchro_count = 0;

	canmsg_received = FALSE;

	PORT_RFCONFIG = 1;
	RF_init(RF_MODULE, &rf_packet_received_callback, &rf_can_received_callback);
	UART_IMPL_write(RF_UART, 'X');
	TIMER3_run_us(1000);	//[1000us = 1ms]
}


void SYNCRF_process_main() {
	//Pour information, si on est pas synchro on allume la led uart
//	bool_e intermediate = !global.is_synchronized;
//	LED_UART = intermediate; //truc bizarres avec assignement direct

	if(canmsg_received)
	{
		//Traitement message CANoverRF
		canmsg_received = FALSE;
	}

	if(request_duration > 0) {
		info_printf("Req : %dms\n", request_duration * DUREE_STEP);
		request_duration = 0;
	}

	if(offset != 0x0FFF) {
		info_printf("Offs: %dms\n", offset * DUREE_STEP);
		offset = 0x0FFF;
	}
}

void _ISR _T3Interrupt()
{
        //Pour garantir une meilleure régularité dans les IT..
        //Chaque ms, on alterne entre la gestion IR et la gestion du moteur.
        static bool_e alternate = FALSE;

        IFS0bits.T3IF = 0;  //On acquite le flag dès le début... en cas de débordement, on pourra re-rentrer dans l'IT sans attendre.

        if(alternate)
            MOTOR_process_it();     //Cette fonction DOIT se terminer en moins d'1ms
        else
            SyncRF_next_step();     //Cette fonction DOIT se terminer en moins d'1ms

        alternate = !alternate;
}




//Varie de 0 à 50*nbmodules, 100 ms / module
volatile Uint8 step_ir = 0;


//#define DUREE_POUR_UN_EMETTEUR	50 //[2ms] => un émetteur est actif pour 100ms.
//@pre Appeler cette fonction toutes les 2ms
static void SyncRF_next_step(void)
{
	//On compte de 0 à TOTAL_STEP_COUNT
	step_ir = (step_ir >= TOTAL_STEP_COUNT - 1)? 0: step_ir+1;

	if(step_ir < TOTAL_STEP_COUNT/2 && is_synchronized)
		LED_RUN = 1;
	else
		LED_RUN = 0;

	//Effectue une demande de synchro
	if(step_ir == TIME_WHEN_SYNCHRO)
		SYNCRF_sendRequest();

	//Si on veut le mode double emetteurs et qu'on a une synchro
	if(is_synchronized)
	{
		Uint8 step_in_period = step_ir % FLASH_CYCLE;

		#if NUMERO_BALISE_EMETTRICE == 1
			if(step_in_period >= NO_FLASH_TIME && step_in_period < PERIODE_FLASH - NO_FLASH_TIME)
				EmissionIR_AUTO();
			else
				EmissionIR_OFF();
		#else
			if(step_in_period >= PERIODE_FLASH + NO_FLASH_TIME && step_in_period < 2*PERIODE_FLASH - NO_FLASH_TIME)
				EmissionIR_AUTO();
			else
				EmissionIR_OFF();
		#endif
	}
	else
		EmissionIR_AUTO();
	//Le décompte du temps est maintenu même si l'on est en simple émetteurs,
	//pour permettre un passage plus tard (= après la synchro) en mode de double émission !
}




void SYNCRF_sendRequest() {
	RF_synchro_request(RF_BROADCAST);

	//Verif de la synchro précédente (on fait la demande avant, au plus près de l'IT, la réponse reviendra dans assez longtemps (80ms), on a le temps
	if(synchro_received)
	{
		//LED_RUN = !LED_RUN; -> on fait clignoter la led_run (mais en IT, pour être au rythme de la synchro)
		LED_USER = 0;
		failed_synchro_count = 0;
		is_synchronized = TRUE; //Synchro reçue
	} 
	else if(failed_synchro_count < (SYNCHRO_TIMEOUT/TOTAL_STEP_COUNT))
	{
		LED_USER = !LED_USER;
		failed_synchro_count++;
	} 
	else
	{
		//Si on a fail trop de fois à suivre, on indique qu'on est plus synchro (et donc passage en mode 1 adversaire, on emet des IR tout le temps)
		is_synchronized = FALSE;
		LED_USER = !LED_USER;
	}
	synchro_received = FALSE;
}

//Cette fonction est appelée pour tout paquet RF qui est arrivé (crc vérifié)
static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size) {
	size = size;
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

static void rf_can_received_callback(CAN_msg_t *msg) {
	if(canmsg_received == FALSE) {
		canmsg_pending = *msg;
		canmsg_received = TRUE;
	} else {
		//overflow, mais le rf est lent (si c'est le cas, alors la boucle while du main est lente aussi)
	}
}
