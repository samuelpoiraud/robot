/*
 *	Club Robot 2015
 *
 *	Fichier : leds.c
 *	Package : Balise receptrice
 *	Description : Gestion des leds
 *	Auteur : Valentin
 */

#include "leds.h"
#include "synchro.h"
#include "QS/QS_maths.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_timer.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_watchdog.h"

typedef struct{
	Uint8 angle; 				//Indice de la led repèrant le mieux chaque adversaire
	Uint8 distance;  			//Nombre de leds à allumer suivant la distance
	bool_e hokuyo_detection;	//A 1 si la détection vient de l'hokuyo
}LEDS_adversary_t;

#define ANGLE_SMALL_FROM_BIG_BOT		(PI4096/2)
#define ANGLE_BIG_FROM_SMALL_BOT		(-PI4096/2)
#define DEPHASING_SMALL_FROM_BIG_BOT	(PI4096)

#define ANGLE_SMALL_FROM_BIG_TOP		(-PI4096/2)
#define ANGLE_BIG_FROM_SMALL_TOP		(PI4096/2)
#define DEPHASING_SMALL_FROM_BIG_TOP	(PI4096)

#define MASK_SIZE						(PI4096/2)

#define HORAIRE							0
#define TRIGO							1
#define PHARE_ROTATE_WAY				(HORAIRE)

#define NB_STEP_BY_LED					20				//Nombre de pas de luminosité possible par LED
#define MAX_LED_VALUE					NB_STEP_BY_LED	//Valeur lumineuse maximal d'une LED
#define NB_LEDS							16				//Le nombre de leds par couleur sur la carte

//Permet de trouver l'indice de la leds à delta de l'index courant
#define FIND_INDEX(index, delta)		((NB_LEDS + (index) + (delta)) % NB_LEDS)

//Permet d'avoir l'indice de la LED la plus proche de l'angle qu'on demande
//Entre chaque led on a un espacement de TWO_PI4096/16 = 1608
//On prend un offset de TWO_PI4096/32 = 804 pour centrer l'intervalle sur la led concernée
//On fait TWO_PI4096 - angle car l'évolution de l'angle est inversée par rapport aux numéros des leds
//    Un angle faible correspond à la led 15 et un angle élevé (ie presque TWO_PI4096) correspond à la led 0
#define ANGLE_TO_LED(angle)				(((Uint8)(16*((TWO_PI4096 - (angle)) + 804)/TWO_PI4096)) % NB_LEDS)

//Prototypes de fonctions
static void LEDS_update(Led_balise_color_e index_color, Uint8 index_step_color);
static void LEDS_search_led_adversaries(LEDS_adversary_t *leds_adv);
static void LEDS_reset_all(Led_balise_color_e color);
static void LEDS_reset_all_color();
static void LEDS_set_all(Led_balise_color_e color);
static Sint16 LEDS_calcul_angle(Sint16 teta); //Centre l'angle entre 0 et TWO_PI4096

static void LEDS_phare(bool_e entrance, Uint8 nb_leds, Led_balise_color_e color);
static void LEDS_phare_on_two_robot(bool_e sync, bool_e entrance, Uint8 nb_leds, Led_balise_color_e color);
static bool_e LEDS_track_adversaries_evitement(bool_e entrance);
static void LEDS_track_adversaries_ir(bool_e entrance, Led_balise_color_e color);
static bool_e LEDS_blink(bool_e entrance, Uint8 nb_clignotement, Led_balise_color_e color);

//Variables globales
static volatile Uint8 display_leds[NB_LEDS_COLOR][NB_LEDS]={{0},{0},{0}}; //Tableau manipulé pour les mise à jour dans la tâche de fond (A manipuler)
static volatile Led_balise_color_e team_color = LEDS_RED;

static volatile bool_e foe_detected = FALSE;
static volatile LEDS_adversary_t foe;

void LEDS_init(){
	WATCHDOG_init();
	LEDS_reset_all_color();
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------Multiplexage------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

void LEDS_mux_process_it(){
	static Led_balise_color_e index_color = 0;
	static Uint8 index_step_color = 0;

	index_step_color++;
	if(index_step_color >= NB_STEP_BY_LED){
		index_step_color = 0;

		index_color++;
		if(index_color >= NB_LEDS_COLOR){
			index_color = 0;
		}
	}

	//Mise à jour des leds
	LEDS_update(index_color, index_step_color);
}

static void LEDS_update(Led_balise_color_e index_color, Uint8 index_step_color){

	if(index_step_color == 0){
		GPIO_WriteBit(LED_SRED,	  0);
		GPIO_WriteBit(LED_SBLUE,  0);
		GPIO_WriteBit(LED_SGREEN, 0);
	}

	//mise à jour des leds (allumé ou éteint)
	GPIO_WriteBit(LED_S0, (display_leds[index_color][0] > index_step_color)?Bit_SET:Bit_RESET);
	GPIO_WriteBit(LED_S1, (display_leds[index_color][1] > index_step_color)?Bit_SET:Bit_RESET);
	GPIO_WriteBit(LED_S2, (display_leds[index_color][2] > index_step_color)?Bit_SET:Bit_RESET);
	GPIO_WriteBit(LED_S3, (display_leds[index_color][3] > index_step_color)?Bit_SET:Bit_RESET);
	GPIO_WriteBit(LED_S4, (display_leds[index_color][4] > index_step_color)?Bit_SET:Bit_RESET);
	GPIO_WriteBit(LED_S5, (display_leds[index_color][5] > index_step_color)?Bit_SET:Bit_RESET);
	GPIO_WriteBit(LED_S6, (display_leds[index_color][6] > index_step_color)?Bit_SET:Bit_RESET);
	GPIO_WriteBit(LED_S7, (display_leds[index_color][7] > index_step_color)?Bit_SET:Bit_RESET);
	GPIO_WriteBit(LED_S8, (display_leds[index_color][8] > index_step_color)?Bit_SET:Bit_RESET);
	GPIO_WriteBit(LED_S9, (display_leds[index_color][9] > index_step_color)?Bit_SET:Bit_RESET);
	GPIO_WriteBit(LED_S10, (display_leds[index_color][10] > index_step_color)?Bit_SET:Bit_RESET);
	GPIO_WriteBit(LED_S11, (display_leds[index_color][11] > index_step_color)?Bit_SET:Bit_RESET);
	GPIO_WriteBit(LED_S12, (display_leds[index_color][12] > index_step_color)?Bit_SET:Bit_RESET);
	GPIO_WriteBit(LED_S13, (display_leds[index_color][13] > index_step_color)?Bit_SET:Bit_RESET);
	GPIO_WriteBit(LED_S14, (display_leds[index_color][14] > index_step_color)?Bit_SET:Bit_RESET);
	GPIO_WriteBit(LED_S15, (display_leds[index_color][15] > index_step_color)?Bit_SET:Bit_RESET);

	if(index_step_color == 0){
		switch(index_color){
			case LEDS_RED:
				GPIO_WriteBit(LED_SRED,	  1);
				GPIO_WriteBit(LED_SBLUE,  0);
				GPIO_WriteBit(LED_SGREEN, 0);
			break;
			case LEDS_BLUE:
				GPIO_WriteBit(LED_SRED,	  0);
				GPIO_WriteBit(LED_SBLUE,  1);
				GPIO_WriteBit(LED_SGREEN, 0);
			break;
			case LEDS_GREEN:
				GPIO_WriteBit(LED_SRED,	  0);
				GPIO_WriteBit(LED_SBLUE,  0);
				GPIO_WriteBit(LED_SGREEN, 1);
			break;
			case NB_LEDS_COLOR: //rien mais évite un bug
			default:
				break;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//----------------------------------------------Manager--------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------


void LEDS_manager_process_it(){

	enum state_e {
		INIT = 0,
		WAIT_BEGIN,
		WAIT_BEGIN_SYNC,
		BEGIN,
		DURING_MATCH,
		AVOIDANCE,
		END_OF_MATCH};
	static enum state_e state = INIT;
	static enum state_e last_state = INIT;
	static enum state_e last_state_for_check_entrance = INIT;
	static bool_e initialized = FALSE;
	bool_e entrance = last_state_for_check_entrance != state || !initialized;
	if(entrance)
		last_state = last_state_for_check_entrance;
	last_state_for_check_entrance = state;
	initialized = TRUE;


	static time32_t time_lost = 0;
	static bool_e sync_lost = FALSE;

	// Entrée dans un nouvelle état donc on réinitialise l'utilisation des couleurs
	if(entrance)
		LEDS_reset_all_color();

	switch(state){
		case INIT :
			sync_lost = FALSE;
			state = WAIT_BEGIN;
			break;

		case WAIT_BEGIN :
			LEDS_phare(entrance, 8, team_color);
			LEDS_track_adversaries_ir(entrance, LEDS_RED);

			if(global.flags.start_of_match)
				state = BEGIN;
			/*else if(SYNCHRO_is_synchronized() == TRUE)
				state = WAIT_BEGIN_SYNC;*/
			break;

		case WAIT_BEGIN_SYNC:
			LEDS_phare_on_two_robot(FALSE, entrance, 6, team_color);
			LEDS_track_adversaries_ir(entrance, LEDS_RED);

			if(global.flags.start_of_match){
				state = BEGIN;
			}else if(SYNCHRO_is_synchronized() == FALSE && sync_lost == FALSE){
				time_lost = global.absolute_time;
				sync_lost = TRUE;
			}else if(SYNCHRO_is_synchronized() == TRUE){
				sync_lost = FALSE;
			}else if(sync_lost && ((global.absolute_time - time_lost) >= 1000)){
				state = WAIT_BEGIN;
			}
			break;

		case BEGIN :
			if(LEDS_blink(entrance, 2, LEDS_RED))
				state = DURING_MATCH;
			break;

		case DURING_MATCH :
			LEDS_track_adversaries_ir(entrance, team_color);
			if(foe_detected)
				state = AVOIDANCE;
			break;

		case AVOIDANCE :
			if(entrance)
				foe_detected = FALSE;
			if(LEDS_track_adversaries_evitement(entrance))
				state = last_state;
			break;

		case END_OF_MATCH :
			LEDS_phare(entrance, 4, team_color);
			break;
	}

}

void LEDS_rf_sync(){
	LEDS_phare_on_two_robot(TRUE, 0, 0, 0);
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//----------------------------------------MAE d'affichage---------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

static void LEDS_phare_on_two_robot(bool_e sync, bool_e entrance, Uint8 nb_leds, Led_balise_color_e color){
	static Uint8 current_index = 0;
	static bool_e flag_time = FALSE;
	static Led_balise_color_e last_color;
	static Uint16 led_mask = 0x00;
	static Sint16 small_dephasing = 0;
	bool_e reason_for_compute = FALSE;
	Uint8 i;

	if(sync){
		current_index = 0;
		return;
	}

	if(entrance){
		current_index = 0;
		flag_time = TRUE;
		last_color = color;
		reason_for_compute = TRUE;
	}else if(last_color != color){
		LEDS_reset_all(last_color);
		last_color = color;
		reason_for_compute = TRUE;
	}

	if(reason_for_compute){
		Sint32 beginLed;
		Sint32 endLed;

		if(QS_WHO_AM_I_get() == BIG_ROBOT){
			if(global.color == BOT_COLOR){
				beginLed = LEDS_calcul_angle(ANGLE_SMALL_FROM_BIG_BOT - MASK_SIZE);
				endLed = LEDS_calcul_angle(ANGLE_SMALL_FROM_BIG_BOT + MASK_SIZE);
			}else{
				beginLed = LEDS_calcul_angle(ANGLE_SMALL_FROM_BIG_TOP - MASK_SIZE);
				endLed = LEDS_calcul_angle(ANGLE_SMALL_FROM_BIG_TOP + MASK_SIZE);
			}
		}else{
			if(global.color == BOT_COLOR){
				beginLed = LEDS_calcul_angle(ANGLE_BIG_FROM_SMALL_BOT - MASK_SIZE);
				endLed = LEDS_calcul_angle(ANGLE_BIG_FROM_SMALL_BOT + MASK_SIZE);
				small_dephasing = DEPHASING_SMALL_FROM_BIG_BOT;
			}else{
				beginLed = LEDS_calcul_angle(ANGLE_BIG_FROM_SMALL_TOP - MASK_SIZE);
				endLed = LEDS_calcul_angle(ANGLE_BIG_FROM_SMALL_TOP + MASK_SIZE);
				small_dephasing = LEDS_calcul_angle(DEPHASING_SMALL_FROM_BIG_TOP);
			}
		}

		beginLed = ANGLE_TO_LED(beginLed);
		endLed = ANGLE_TO_LED(endLed);
		small_dephasing = ANGLE_TO_LED(small_dephasing);

		led_mask = 0x00;
		if(beginLed >=  0 && beginLed < NB_LEDS && endLed >= 0 && endLed < NB_LEDS){
			Uint8 index = beginLed;
			led_mask |= 1 << index;
			do{
				index++;
				if(index > NB_LEDS)
					index = 0;
				led_mask |= 1 << index;
			}while(index != endLed);
		}
	}


	if(flag_time){
		LEDS_reset_all(color);

		for(i=0; i<nb_leds; i++){
			if(QS_WHO_AM_I_get() == BIG_ROBOT){
#if PHARE_ROTATE_WAY == HORAIRE
				if(led_mask & (1 << FIND_INDEX(current_index, i)))
					display_leds[color][FIND_INDEX(current_index, i)] = MAX_LED_VALUE - ((nb_leds - (i+1))*MAX_LED_VALUE/nb_leds);
#else
				if(led_mask & (1 << FIND_INDEX(current_index, -i)))
					display_leds[color][FIND_INDEX(current_index, -i)] = MAX_LED_VALUE - ((nb_leds - (i+1))*MAX_LED_VALUE/nb_leds);
#endif
			}else{
#if PHARE_ROTATE_WAY == HORAIRE
				if(led_mask & (1 << FIND_INDEX(current_index + small_dephasing, i)))
					display_leds[color][FIND_INDEX(current_index + small_dephasing, i)] = MAX_LED_VALUE - ((nb_leds - (i+1))*MAX_LED_VALUE/nb_leds);
#else
				if(led_mask & (1 << FIND_INDEX(current_index + small_dephasing, -i)))
					display_leds[color][FIND_INDEX(current_index + small_dephasing, -i)] = MAX_LED_VALUE - ((nb_leds - (i+1))*MAX_LED_VALUE/nb_leds);
#endif
			}
		}

#if PHARE_ROTATE_WAY == HORAIRE
		if(current_index >= NB_LEDS)
			current_index = 0;
		else
			current_index++; //On incrémente l'indice courant pour faire avancer le phare
#else
		if(current_index == 0)
			current_index = NB_LEDS - 1;
		else
			current_index--; //On décrémente l'indice courant pour faire reculer le phare

#endif

		WATCHDOG_create_flag(25, &flag_time);
	}
}

static void LEDS_phare(bool_e entrance, Uint8 nb_leds, Led_balise_color_e color){
	static Uint8 current_index = 0;
	static bool_e flag_time = FALSE;
	static Led_balise_color_e last_color;
	Uint8 i;

	if(entrance){
		current_index = 0;
		flag_time = TRUE;
		last_color = color;
	}else if(last_color != color){
		LEDS_reset_all(last_color);
		last_color = color;
	}

	if(flag_time){
		LEDS_reset_all(color);

		for(i=0; i<nb_leds; i++){
#if PHARE_ROTATE_WAY == HORAIRE
			display_leds[color][FIND_INDEX(current_index, i)] = MAX_LED_VALUE - ((nb_leds - (i+1))*MAX_LED_VALUE/nb_leds);
#else
			display_leds[color][FIND_INDEX(current_index, -i)] = MAX_LED_VALUE - ((nb_leds - (i+1))*MAX_LED_VALUE/nb_leds);
#endif
		}

#if PHARE_ROTATE_WAY == HORAIRE
		if(current_index >= NB_LEDS)
			current_index = 0;
		else
			current_index++; //On incrémente l'indice courant pour faire avancer le phare
#else
		if(current_index == 0)
			current_index = NB_LEDS - 1;
		else
			current_index--; //On décrémente l'indice courant pour faire reculer le phare

#endif

		WATCHDOG_create_flag(25, &flag_time);
	}
}

static bool_e LEDS_blink(bool_e entrance, Uint8 nb_clignotement, Led_balise_color_e color){
	static Uint8 step = 0;
	static Uint16 count = 0;
	static bool_e flag_time = FALSE;
	static Led_balise_color_e last_color;

	if(entrance){
		step = 0;
		count = 0;
		last_color = color;
	}else if(last_color != color){
		LEDS_reset_all(last_color);
		last_color = color;
	}

	switch(step){
		case 0:
			LEDS_set_all(color);
			step = 1;
			WATCHDOG_create_flag(200, &flag_time);
			break;

		case 1:
			if(flag_time)
				step = 2;
			break;

		case 2:
			LEDS_reset_all(color);
			step = 3;
			WATCHDOG_create_flag(200, &flag_time);
			break;

		case 3:
			if(flag_time){
				count++;
				if(count >= nb_clignotement){
					step = 0;
					count = 0;
					return TRUE;
				}
				step = 0;
			}
			break;
	}
	return FALSE;
}


//On allume les leds les plus proches de l'adversaire à éviter
//Fonction appelée uniquement lors d'un évitement
//Couleur d'affichage: RED
static bool_e LEDS_track_adversaries_evitement(bool_e entrance){
	static bool_e flag_time = FALSE;
	static Uint8 step = 0;
	Uint8 i;

	if(entrance)
		step = 0;

	switch(step){
		case 0:
			LEDS_reset_all_color();
			if(foe.hokuyo_detection ==  FALSE){
				display_leds[LEDS_RED][foe.angle] = MAX_LED_VALUE; //On allume la led la plus centrée
				for(i=1; i<foe.distance; i++){ //allumage des leds sur chaque côté si besoin (ie si le robot adversaire est proche)
					display_leds[LEDS_RED][FIND_INDEX(foe.angle, i)] = (4-i)*MAX_LED_VALUE/4;
					display_leds[LEDS_RED][FIND_INDEX(foe.angle, -i)] = (4-i)*MAX_LED_VALUE/4;
				}
			}else{
				display_leds[LEDS_BLUE][foe.angle] = MAX_LED_VALUE; //On allume la led la plus centrée
				for(i=1; i<foe.distance; i++){ //allumage des leds sur chaque côté si besoin (ie si le robot adversaire est proche)
					display_leds[LEDS_BLUE][FIND_INDEX(foe.angle, i)] = (4-i)*MAX_LED_VALUE/4;
					display_leds[LEDS_BLUE][FIND_INDEX(foe.angle, -i)] = (4-i)*MAX_LED_VALUE/4;
				}

				display_leds[LEDS_GREEN][foe.angle] = MAX_LED_VALUE; //On allume la led la plus centrée
				for(i=1; i<foe.distance; i++){ //allumage des leds sur chaque côté si besoin (ie si le robot adversaire est proche)
					display_leds[LEDS_GREEN][FIND_INDEX(foe.angle, i)] = (4-i)*MAX_LED_VALUE/4;
					display_leds[LEDS_GREEN][FIND_INDEX(foe.angle, -i)] = (4-i)*MAX_LED_VALUE/4;
				}
			}
			step = 1;
			WATCHDOG_create_flag(2000, &flag_time);
			break;

		case 1:
			if(flag_time){
				step = 0;
				return TRUE;
			}
			break;
	}
	return FALSE;
}


//On allume les leds les plus proches des adversaires suivant l'angle et leur distance
//Plus le robot est plus, plus il y a de leds allumées.
//Couleur d'affichage : team_color
static void LEDS_track_adversaries_ir(bool_e entrance, Led_balise_color_e color){
	static bool_e flag_time = FALSE;
	static Led_balise_color_e last_color;

	if(entrance){
		flag_time = TRUE;
		last_color = color;
	}else if(last_color != color){
		LEDS_reset_all(last_color);
		last_color = color;
	}

	if(flag_time){
		Uint8 i, j;
		LEDS_adversary_t leds_adversary[ADVERSARY_NUMBER];

		LEDS_search_led_adversaries(leds_adversary);
		//On regarde si les adversaires sont là
		if(SYNCHRO_get_warner_foe1_is_rf_unreacheable())
			leds_adversary[ADVERSARY_1].distance = 0;
		if(SYNCHRO_get_warner_foe2_is_rf_unreacheable())
			leds_adversary[ADVERSARY_2].distance = 0;

		LEDS_reset_all(color); //On efface les données précédemment enregistrées

		//Mise à jour du tableau de leds
		for(i=0; i<=ADVERSARY_2; i++){
			if(leds_adversary[i].distance != 0)  //Si la balise émetrice est allumée
				display_leds[color][leds_adversary[i].angle] = MAX_LED_VALUE; //On allume la led la plus centrée
			for(j=1; j<leds_adversary[i].distance; j++){ //allumage des leds sur chaque côté si besoin (ie si le robot adversaire est proche)
				display_leds[color][FIND_INDEX(leds_adversary[i].angle, j)] = (4-j)*MAX_LED_VALUE/4;
				display_leds[color][FIND_INDEX(leds_adversary[i].angle, -j)] = (4-j)*MAX_LED_VALUE/4;
			}
		}

		WATCHDOG_create_flag(100, &flag_time);
	}
}


//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//----------------------------------------------Autres---------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

//Fonction permettant de rechercher la led qui repère le mieux chaque adversaire + nombre de leds à allumer
static void LEDS_search_led_adversaries(LEDS_adversary_t *leds_adv){
	Uint8 i;

	for(i = 0; i < ADVERSARY_NUMBER; i++){

		Sint16 angle;
		adversary_location_t* adv = (adversary_location_t*)BRAIN_get_adversary_location();
		///RECHERCHE DE L'ANGLE
		angle = LEDS_calcul_angle(adv[i].angle);

		leds_adv[i].angle = ANGLE_TO_LED(angle);

		if(leds_adv[i].angle == NB_LEDS)  //on gère l'effet de bord pour la led 0
			leds_adv[i].angle = 0;

		///RECHERCHE DE LA DISTANCE
		//La distance exprimé en cm/2. C'est à dire que adv.distance*20 donne la distance en mm.
		if(adv[i].distance <= 25) //Inférieur à 50 cm
			leds_adv[i].distance = 4;
		else if(adv[i].distance <= 50)//Inférieur à 100 cm
			leds_adv[i].distance = 3;
		else if(adv[i].distance <= 100) //Inférieur à 200 cm
			leds_adv[i].distance = 2;
		else
			leds_adv[i].distance = 1;

		leds_adv[i].hokuyo_detection = FALSE;
	}
}

void LEDS_set_team_color(Led_balise_color_e color){
	team_color = color;
}

void LEDS_set_infos_evitement(CAN_msg_t * msg){

	///RECHERCHE DE L'ANGLE
	Sint16 angle = LEDS_calcul_angle(msg->data.strat_prop_foe_detected.angle);

	foe.angle = ANGLE_TO_LED(angle);

	if(foe.angle == NB_LEDS)  //on gère l'effet de bord pour la led 0
		foe.angle = 0;

	foe.distance = 4;

	foe.hokuyo_detection = msg->data.strat_prop_foe_detected.hokuyo_detection;

	foe_detected = TRUE;
}

static void LEDS_reset_all(Led_balise_color_e color){
	Uint8 i;
	for(i=0; i<NB_LEDS; i++){
		display_leds[color][i] = 0;
	}
}

static void LEDS_reset_all_color(){
	LEDS_reset_all(LEDS_RED);
	LEDS_reset_all(LEDS_BLUE);
	LEDS_reset_all(LEDS_GREEN);
}

static void LEDS_set_all(Led_balise_color_e color){
	Uint8 i;
	for(i=0; i<NB_LEDS; i++){
		display_leds[color][i] = MAX_LED_VALUE;
	}
}

static Sint16 LEDS_calcul_angle(Sint16 teta){
	while(teta < 0)
		teta += TWO_PI4096;
	while(teta > TWO_PI4096)
		teta -= TWO_PI4096;
	return teta;
}
