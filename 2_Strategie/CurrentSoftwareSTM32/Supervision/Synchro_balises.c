#include "Synchro_balises.h"

#include "../QS/QS_rf.h"
#include "../QS/QS_timer.h"
#include "../QS/QS_who_am_i.h"
#include "../QS/QS_outputlog.h"
#include "../config/config_pin.h"
#include "../QS/impl/QS_uart_impl.h"

#ifdef SYNCHROBALISE_TIMER
	#define TIMER_SRC_TIMER_ID SYNCHROBALISE_TIMER
#endif
#include "../QS/QS_setTimerSource.h"

#define COMPTEUR_USEC_PER_TICK 100 //[us]
#define MS_TO_COMPTEUR(ms) (ms*1000/COMPTEUR_USEC_PER_TICK)

#define TIME_PER_MODULE           MS_TO_COMPTEUR(100)  //[ms] 100ms
#define PERIODE_SIGNAL_SYNCHRO    MS_TO_COMPTEUR(200) //[ms] 200 ms, signal synchro toutes les 200ms
#define DUREE_SIGNAL_SYNCHRO      MS_TO_COMPTEUR(2)  //[ms] 2ms, signal synchro pendant 2ms

#define COMPTEUR_MAX              RF_MODULE_COUNT*TIME_PER_MODULE
#define TIME_WHEN_SYNCHRO         TIME_PER_MODULE*RF_get_module_id()  //valeur compteur quand demander la synchro

#define LAST_REPLY_TIMEOUT 10000  //[ms] temps avant de considérer le robot maitre (BIG) comme éteint (dans ce cas, SMALL passe en maitre)
#define LAST_SYNCHRO_TIMEOUT 1000  //[ms] temps avant de considérer un problème d'un module quand on ne detecte plus rien de lui pendant ce temps


static bool_e canmsg_received;
static CAN_msg_t canmsg_pending;

static Sint16 offset;
static time32_t last_received_reply; //si ça fait longtemps, BIG est off et ne répond plus aux demandes

//temps depuis la dernière activité RF, permet de savoir si les autres modules RF sont là
static time32_t last_activity_time[RF_MODULE_COUNT] = {0};

static bool_e SEND_REQ;
static bool_e REPLY_REQ;

static Uint16 time_base = 0;

static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size);
static void rf_can_received_callback(CAN_msg_t *msg);
static void update_rfmodule_here();

void TIMER_SRC_TIMER_interrupt() {
	TIMER_SRC_TIMER_resetFlag();

	time_base++;
	if(time_base >= COMPTEUR_MAX) {
		time_base=0;
	}

	SYNCHRO_BEACON = ((time_base % PERIODE_SIGNAL_SYNCHRO) < PERIODE_SIGNAL_SYNCHRO/2);  //Actif la moitié du temps inférieure (entre 0 et PERIODE_SIGNAL_SYNCHRO / 2)

	//Un timeslot pour chaque module différent, pour éviter les collisions lorsque tout les modules sont presque synchro
	if(SEND_REQ && time_base == TIME_WHEN_SYNCHRO) {
		RF_synchro_request(RF_BROADCAST);
	}

}

void SYNCHRO_init() {
	TIMER_SRC_TIMER_init();

	canmsg_received = FALSE;
	offset = 0x0FFF;
	last_received_reply = global.env.absolute_time;

	PIN_RF_CONFIG = 1;

	if(QS_WHO_AM_I_get() == BIG_ROBOT) {
		SEND_REQ = FALSE;
		REPLY_REQ = TRUE;
		RF_init(RF_BIG, &rf_packet_received_callback, &rf_can_received_callback);
	} else {
		SEND_REQ = TRUE;		//Pour voir si il y a Le gros robot
		REPLY_REQ = FALSE;
		RF_init(RF_SMALL, &rf_packet_received_callback, &rf_can_received_callback);
	}

	// X indique la fin de la config => passage en mode normal de transmission / reception (la pin de config doit être à 1)
	UART_IMPL_write(RF_UART, 'X');

	TIMER_SRC_TIMER_start_us(COMPTEUR_USEC_PER_TICK);
}

void SYNCHRO_process_main()
{
	//Message CAN reçu => passage à environnement.c
	if(canmsg_received) {
		//ENV_process_can_msg(&canmsg_pending);
		ENV_process_can_msg(&canmsg_pending, TRUE, TRUE, FALSE, FALSE); //Renvoi sur le bus CAN et U1

		canmsg_received = FALSE;
	}

	//Si on voit pas de réponse pendant plus de LAST_REPLY_TIMEOUT ms, on répond au demandes de synchro (si BIG_ROBOT pas là)
	if(REPLY_REQ == FALSE && RF_get_module_id() == RF_SMALL && last_received_reply + LAST_REPLY_TIMEOUT <= global.env.absolute_time) {
		REPLY_REQ = TRUE;
		debug_printf("La derniere réponse reçu de BIG_ROBOT est trop vieille, passage en maitre\n");
	}

	//Met à jour balise_here
	update_rfmodule_here();

	//Pour debug
	if(offset != 0x0FFF) {
		//info_printf("Offset: %d\n", offset);
		offset = 0x0FFF;
	}
}

static Sint16 wrap_timebase(Sint16 val) {
	if(val >= COMPTEUR_MAX)
		return val - COMPTEUR_MAX;
	else if(val >= 0)
		return val;

	return val + COMPTEUR_MAX;
}

static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size) {
	if(header.type == RF_PT_SynchroRequest) {
		if(REPLY_REQ && for_me) {
			Sint16 offset, expected_time;

			expected_time = TIME_PER_MODULE * header.sender_id;
			offset = time_base - expected_time;

			RF_synchro_response(header.sender_id, offset);
			LED_BEACON_IR_GREEN = !LED_BEACON_IR_GREEN;
		}
	} else if(QS_WHO_AM_I_get() == SMALL_ROBOT && header.type == RF_PT_SynchroResponse) {
		//BIG_ROBOT nous à répondu, on maj notre base de temps
		if(for_me) {
			Sint16 fullOffset = (data[0] | data[1] << 8);
			offset = fullOffset - (wrap_timebase(((Sint16)time_base) - TIME_WHEN_SYNCHRO) / 2);

			time_base = wrap_timebase(((Sint16)time_base) + offset);

			LED_BEACON_IR_GREEN = !LED_BEACON_IR_GREEN;
		}

		//On a vu une réponse d'un autre module que nous (on ne reçoit pas ce qu'on envoie) => BIG_ROBOT est là
		if(REPLY_REQ) {
			REPLY_REQ = FALSE;
			debug_printf("Réponse reçue de BIG_ROBOT, passage en esclave\n");
		}
		last_received_reply = global.env.absolute_time;
	}

	//Même si le message n'est pas pour nous, il suffit de voir qu'un module est actif pour l'enregistrer
	if(header.sender_id < RF_MODULE_COUNT)
		last_activity_time[header.sender_id] = global.env.absolute_time;
}

static void rf_can_received_callback(CAN_msg_t *msg) {
	//Le message CAN reçu est forcément pour nous => on le bufferise pour l'envoyer à environnement.c dans la boucle main (SYNCHRO_process_main)
	if(canmsg_received == FALSE) {
		canmsg_pending = *msg;
		canmsg_received = TRUE;
	} else {
		//overflow, mais le rf est lent (si c'est le cas, alors la boucle while du main est lente aussi)
	}
}

static void update_rfmodule_here() {
	Uint8 module_id;
	bool_e someone_not_here = FALSE;

	for(module_id = 0; module_id < RF_MODULE_COUNT; module_id++) {
		//On est nous même là
		if(RF_get_module_id() != module_id) {
			if(last_activity_time[module_id] + LAST_SYNCHRO_TIMEOUT >= global.env.absolute_time) {
				someone_not_here = TRUE;
			}
		}
	}

	if(someone_not_here)
		LED_BEACON_IR_RED = 1;
	else
		LED_BEACON_IR_RED = 0;
}
