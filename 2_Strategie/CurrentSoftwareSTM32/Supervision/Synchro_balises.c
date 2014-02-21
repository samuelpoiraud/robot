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

#define LAST_REPLY_TIMEOUT 10000  //temps avant de considérer le robot maitre (pierre) comme éteint (dans ce cas, guy passe en maitre)
#define LAST_SYNCHRO_TIMEOUT 10000  //temps avant de considérer une balise comme éteinte

#define COMPTEUR_USEC_PER_TICK 100
#define MS_TO_COMPTEUR(ms) (ms*1000/COMPTEUR_USEC_PER_TICK)

#define TIME_PER_MODULE           MS_TO_COMPTEUR(100)  //100ms
#define PERIODE_SIGNAL_SYNCHRO    MS_TO_COMPTEUR(200) //200 ms, signal synchro toutes les 200ms
#define DUREE_SIGNAL_SYNCHRO      MS_TO_COMPTEUR(2)  //2ms, signal synchro pendant 2ms

#define COMPTEUR_MAX              RF_MODULE_COUNT*TIME_PER_MODULE
#define TIME_WHEN_SYNCHRO         TIME_PER_MODULE*RF_get_module_id()  //valeur compteur quand demander la synchro


//Les balises sont elles là ? Informatif, non utilisé dans ce code (sauf pour leur maj)
bool_e balise_here[2] = {FALSE, FALSE};

static CAN_msg_t canmsg_pending;
static bool_e canmsg_received = FALSE;
static Sint16 offset;
static time32_t last_received_reply = 0; //si ça fait longtemps, pierre est off et ne répond plus aux demandes

//temps depuis la dernière requête de synchro, permet de savoir si les balises sont là
static time32_t last_req_time[RF_MODULE_COUNT];

static bool_e SEND_REQ;
static bool_e REPLY_REQ;

static Uint16 time_base = 0;

static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size);
static void rf_can_received_callback(CAN_msg_t *msg);
static void update_foe_here();

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

	PIN_RF_CONFIG = 1;

	if(QS_WHO_AM_I_get() == PIERRE) {
		SEND_REQ = FALSE;
		REPLY_REQ = TRUE;
		RF_init(RF_PIERRE, &rf_packet_received_callback, &rf_can_received_callback);
	} else {
		SEND_REQ = TRUE;		//Pour voir si il y a PIERRE
		REPLY_REQ = FALSE;
		RF_init(RF_GUY, &rf_packet_received_callback, &rf_can_received_callback);
	}

	// X indique la fin de la config => passage en mode normal de transmission / reception (la pin de config doit être à 1)
	UART_IMPL_write(RF_UART, 'X');

	TIMER_SRC_TIMER_start_us(COMPTEUR_USEC_PER_TICK);
}

void SYNCHRO_process_main()
{
	static Uint16 compteur_last;

	//Message CAN reçu => passage à environnement.c
	if(canmsg_received) {
		//ENV_process_can_msg(&canmsg_pending);
		ENV_process_can_msg(&canmsg_pending, TRUE, TRUE, FALSE, FALSE); //Renvoi sur le bus CAN et U1

		canmsg_received = FALSE;
	}

	//Si on voit pas de réponse pendant plus de LAST_REPLY_TIMEOUT ms, on répond au demandes e synchro (si pierre pas là)
	if(REPLY_REQ == FALSE && RF_get_module_id() == RF_GUY && last_received_reply + LAST_REPLY_TIMEOUT <= global.env.absolute_time) {
		REPLY_REQ = TRUE;
		debug_printf("La derniere réponse reçu de PIERRE est trop vieille, passage en maitre\n");
	}

	//Met à jour balise_here
	update_foe_here();

	//Pour debug
	if(offset != 0x0FFF) {
		info_printf("Offset: %d\n", offset);
		offset = 0x0FFF;
	}

	//Pour débuggage: si les 2 cartes comptent en même temps, la synchro est ok
	if(compteur_last != time_base/1000) {
		compteur_last = time_base/1000;

		//debug_printf("Compteur: %u\n", time_base);
	}
}

static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size) {
	if(header.type == RF_PT_SynchroRequest) {
		if(REPLY_REQ && for_me) {
			Sint16 offset, expected_time;

			expected_time = TIME_PER_MODULE * header.sender_id;
			offset = time_base - expected_time;

			RF_synchro_response(header.sender_id, offset);
			LED_SELFTEST = !LED_SELFTEST;
			#warning LED_SELFTEST utilise pour debuggage
		}
		//Même si le message n'est pas pour nous, suffi de voir qu'un module est actif pour l'enregistrer
		last_req_time[header.target_id] = global.env.absolute_time;
		update_foe_here();
	} else if(QS_WHO_AM_I_get() == GUY && header.type == RF_PT_SynchroResponse) {
		//Pierre nous à répondu, on maj notre base de temps
		if(for_me) {
			Sint16 fullOffset = (data[0] | data[1] << 8);
			offset = time_base - TIME_WHEN_SYNCHRO;
			time_base = (time_base + fullOffset - (offset >> 1)) % COMPTEUR_MAX;
		}

		//On a vu une réponse d'un autre module que nous (on ne reçoit pas ce qu'on envoie) => Pierre est là
		if(REPLY_REQ) {
			REPLY_REQ = FALSE;
			debug_printf("Réponse reçue de PIERRE, passage en esclave\n");
		}
		last_received_reply = global.env.absolute_time;
	}
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

static void update_foe_here() {
	balise_here[0] = (last_req_time[RF_FOE1] + LAST_SYNCHRO_TIMEOUT >= global.env.absolute_time);
	balise_here[1] = (last_req_time[RF_FOE2] + LAST_SYNCHRO_TIMEOUT >= global.env.absolute_time);
}
