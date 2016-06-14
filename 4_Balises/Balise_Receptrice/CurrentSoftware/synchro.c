#include "synchro.h"

#include "QS/QS_rf.h"
#include "QS/QS_timer.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_outputlog.h"
#include "QS/impl/QS_uart_impl.h"
#include "QS/QS_ports.h"
#include "eyes.h"
#include "leds.h"

#define TIMER_SRC_TIMER_ID SYNCHROBALISE_TIMER
#include "QS/QS_setTimerSource.h"

#define COMPTEUR_USEC_PER_TICK 100 //[us]
#define MS_TO_COMPTEUR(ms) (ms*1000/COMPTEUR_USEC_PER_TICK)

#define TIME_PER_MODULE           MS_TO_COMPTEUR(100)  //[ms] 100ms
#define PERIODE_SIGNAL_SYNCHRO    MS_TO_COMPTEUR(200) //[ms] 200 ms, signal synchro toutes les 200ms
#define DUREE_SIGNAL_SYNCHRO      MS_TO_COMPTEUR(2)  //[ms] 2ms, signal synchro pendant 2ms

#define TIME_SYNC_LED			  MS_TO_COMPTEUR(2000)

#define COMPTEUR_MAX              RF_MODULE_COUNT*TIME_PER_MODULE
#define TIME_WHEN_SYNCHRO         TIME_PER_MODULE*RF_get_module_id()  //valeur compteur quand demander la synchro

#define LAST_REPLY_TIMEOUT 10000  //[ms] temps avant de considérer le robot maitre (BIG) comme éteint (dans ce cas, SMALL passe en maitre)
#define LAST_SYNCHRO_TIMEOUT 1000  //[ms] temps avant de considérer un problème d'un module quand on ne detecte plus rien de lui pendant ce temps

#define PWM_THRESHOLD_ON_FOE_BEACON		50		//Au delà ce % de PWM, on considère que la batterie est faible sur les balises.
#define PERIOD_PRINT_LOW_BAT_ON_BEACON	10000	//[ms]

static Sint16 offset;
static time32_t last_received_reply; //si ça fait longtemps, BIG est off et ne répond plus aux demandes

//temps depuis la dernière activité RF, permet de savoir si les autres modules RF sont là
static time32_t last_activity_time[RF_MODULE_COUNT] = {0};

static bool_e SEND_REQ;
static bool_e REPLY_REQ;
static volatile bool_e is_synchronized = FALSE;
static volatile bool_e led_synchro = FALSE;

static Uint16 time_base = 0;


static void rf_packet_received_callback(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size);
static void update_rfmodule_here();

void TIMER_SRC_TIMER_interrupt() {
	static adversary_e last_adv = -1;
	static Uint8 countTime = 0; // Unité : [400ms]
	adversary_e adv = -1;
	TIMER_SRC_TIMER_resetFlag();

	time_base++;
	if(time_base >= COMPTEUR_MAX) {
		time_base=0;
		countTime++;
		if(countTime >= 5){
			countTime = 0;
			LEDS_rf_sync();
		}
	}


	if(time_base % PERIODE_SIGNAL_SYNCHRO < PERIODE_SIGNAL_SYNCHRO/2)
		adv = ADVERSARY_1;
	else
		adv = ADVERSARY_2;


	if(adv != last_adv){
		last_adv = adv;
		EYES_set_adversary(last_adv);

		if(is_synchronized && adv >= 0){
			led_synchro = !led_synchro;
			GPIO_WriteBit(LED_G_SYNC, led_synchro);
		}else
			GPIO_ResetBits(LED_G_SYNC);
		is_synchronized = FALSE;
	}

	//Un timeslot pour chaque module différent, pour éviter les collisions lorsque tout les modules sont presque synchro
	if(SEND_REQ && time_base == TIME_WHEN_SYNCHRO) {
		RF_synchro_request(RF_BROADCAST);
		//debug_printf("Ask synchro\n");
	}
}

void SYNCHRO_init() {
	TIMER_SRC_TIMER_init();

	offset = 0x0FFF;
	last_received_reply = global.absolute_time;

	GPIO_SetBits(RF_CONFIG);

	if(QS_WHO_AM_I_get() == BIG_ROBOT) {
		SEND_REQ = FALSE;
		REPLY_REQ = TRUE;
		RF_init(RF_BIG, &rf_packet_received_callback, NULL);
	} else {
		SEND_REQ = TRUE;		//Pour voir si il y a Le gros robot
		REPLY_REQ = FALSE;
		RF_init(RF_SMALL, &rf_packet_received_callback, NULL);
	}

	// X indique la fin de la config => passage en mode normal de transmission / reception (la pin de config doit être à 1)
#ifndef CONFIG_RF_RC1240
	UART_IMPL_write(RF_UART, 'X');
#endif
	debug_printf("Synchro init\n");

	TIMER_SRC_TIMER_start_us(COMPTEUR_USEC_PER_TICK);
}



void SYNCHRO_process_main()
{
	//Si on voit pas de réponse pendant plus de LAST_REPLY_TIMEOUT ms, on répond au demandes de synchro (si BIG_ROBOT pas là)
	if(REPLY_REQ == FALSE && RF_get_module_id() == RF_SMALL && last_received_reply + LAST_REPLY_TIMEOUT <= global.absolute_time) {
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
			is_synchronized = TRUE;
			//debug_printf("Synchro_request for me from %d: send response\n", header.sender_id);
		}
	} else if(QS_WHO_AM_I_get() == SMALL_ROBOT && header.type == RF_PT_SynchroResponse) {
		//BIG_ROBOT nous à répondu, on maj notre base de temps
		if(for_me) {
			TIMER_SRC_TIMER_DisableIT();
			Sint16 fullOffset = (data[0] | (data[1] << 8));
			offset = fullOffset - (wrap_timebase(((Sint16)time_base) - TIME_WHEN_SYNCHRO) / 2);

			//On ajoute l'offset au compteur + 6ms. Cette valeur de 6ms a été choisie après des tests.
			//Cette valeur de 6ms permet d'ajuster la synchro des deux robots très précisément.
			time_base = wrap_timebase(((Sint16)time_base) + offset + MS_TO_COMPTEUR(6));
			TIMER_SRC_TIMER_EnableIT();
			is_synchronized = TRUE;
			//debug_printf("Synchro_response for me\n");
		}

		//On a vu une réponse d'un autre module que nous (on ne reçoit pas ce qu'on envoie) => BIG_ROBOT est là
		if(REPLY_REQ) {
			REPLY_REQ = FALSE;
			debug_printf("Réponse reçue de BIG_ROBOT, passage en esclave\n");
		}
		last_received_reply = global.absolute_time;
	}

	//Même si le message n'est pas pour nous, il suffit de voir qu'un module est actif pour l'enregistrer
	if(header.sender_id < RF_MODULE_COUNT){
		last_activity_time[header.sender_id] = global.absolute_time;
		//debug_printf("Get msg of %d\n", header.sender_id);
	}
}

static bool_e warner_foe1_is_rf_unreacheable = FALSE;
static bool_e warner_foe2_is_rf_unreacheable = FALSE;

bool_e SYNCHRO_get_warner_foe1_is_rf_unreacheable(void)
{
	return warner_foe1_is_rf_unreacheable;
}

bool_e SYNCHRO_get_warner_foe2_is_rf_unreacheable(void)
{
	return warner_foe2_is_rf_unreacheable;
}

bool_e SYNCHRO_is_synchronized(){
	return is_synchronized;
}

static void update_rfmodule_here()
{

	Uint8 module_id;
	bool_e someone_not_here = FALSE;
	bool_e warner_foe1_is_rf_unreacheable_temp = FALSE;	// On suppose qu'on n'a pas de soucis de liaison
	bool_e warner_foe2_is_rf_unreacheable_temp = FALSE;

	//debug_printf("global_time= %ld,    last_activity foe1 = %ld,  last_activity foe2 = %ld\n", global.absolute_time, last_activity_time[RF_FOE1], last_activity_time[RF_FOE2]);
	for(module_id = 0; module_id < RF_MODULE_COUNT; module_id++)
	{
		//On est nous même là
		if(RF_get_module_id() != module_id)
		{
			if(global.absolute_time - last_activity_time[module_id] >= LAST_SYNCHRO_TIMEOUT)
			{
				someone_not_here = TRUE;
				switch(module_id)
				{
					case RF_FOE1:
						warner_foe1_is_rf_unreacheable_temp = TRUE;
						break;
					case RF_FOE2:
						warner_foe2_is_rf_unreacheable_temp = TRUE;
						break;
					default:
						break;
				}
			}
		}
	}

	if(someone_not_here){
		GPIO_SetBits(LED_R_SYNC);
	}else{
		GPIO_ResetBits(LED_R_SYNC);
	}
	warner_foe1_is_rf_unreacheable = warner_foe1_is_rf_unreacheable_temp;
	warner_foe2_is_rf_unreacheable = warner_foe2_is_rf_unreacheable_temp;
}
