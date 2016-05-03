#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "QS/QS_all.h"
#include "Supervision/SD/SD.h"
#include "avoidance.h"


	typedef enum{
		NO_ELEMENT,
		OUR_ELEMENT,
		ADV_ELEMENT,
		NEUTRAL_ELEMENT
	}ELEMENTS_property_e;


	typedef struct{
		GEOMETRY_point_t pos;
		ELEMENTS_property_e property;
	}COQUILLAGES_t;

	typedef enum{
	   NO_COQUILLAGES_CONFIG = 0,
	   CONFIG_COQUILLAGES_1,
	   CONFIG_COQUILLAGES_2,
	   CONFIG_COQUILLAGES_3,
	   CONFIG_COQUILLAGES_4,
	   CONFIG_COQUILLAGES_5
	}COQUILLAGES_config_e;

	typedef enum{
		//Flags coquillages (Veuillez laisser les flags coquillages ici)
		COQUILLAGE_AWAY_ONE,
		COQUILLAGE_AWAY_TWO,
		COQUILLAGE_AWAY_THREE,
		COQUILLAGE_AWAY_FOUR,
		COQUILLAGE_AWAY_FIVE,
		COQUILLAGE_AWAY_ROCK_ONE,
		COQUILLAGE_AWAY_ROCK_TWO,
		COQUILLAGE_AWAY_ROCK_THREE,
		COQUILLAGE_NEUTRAL_ONE,
		COQUILLAGE_NEUTRAL_TWO,
		COQUILLAGE_NEUTRAL_THREE,
		COQUILLAGE_NEUTRAL_FOUR,
		COQUILLAGE_NEUTRAL_FIVE,
		COQUILLAGE_NEUTRAL_SIX,
		COQUILLAGE_HOME_ONE,
		COQUILLAGE_HOME_TWO,
		COQUILLAGE_HOME_THREE,
		COQUILLAGE_HOME_FOUR,
		COQUILLAGE_HOME_FIVE,
		COQUILLAGE_HOME_ROCK_ONE,
		COQUILLAGE_HOME_ROCK_TWO,
		COQUILLAGE_HOME_ROCK_THREE,
		COQUILLAGE_NB, //Sert simplement à la gestion des flags coquillages
		COQUILLAGES_ATTAQUE,
		COQUILLAGES_DEFENSE,

		//Eléments pris
		FIRST_DOOR_CLOSED,
		SECOND_DOOR_CLOSED,
		OUR_START_ZONE_BLOC_TAKEN,		//on a pris notre bloc de 4
		ADV_START_ZONE_BLOC_TAKEN,		//on a pris le bloc de 4 adv
		OUR_DUNE_BLOC_TAKEN,			//on a pris notre bloc de 8
		ADV_DUNE_BLOC_TAKEN,			//on a pris le bloc de 8 adv
		FISHS_TAKEN,				    //poisson embarqués dans le robot
		DUNE_TAKEN,					    //on a pris la dune

		//Apocalypse
		OUR_BLOC_PRESENT,			//on a vu (au scan) notre bloc de 8
		OUR_BLOC_ABSENT,			//on a PAS vu (au scan) notre bloc de 8
		ADV_BLOC_PRESENT,			//on a vu (au scan) le bloc de 8 adv
		ADV_BLOC_ABSENT,			//on a PAS vu (au scan) le bloc de 8 adv
		APOCALYPSE_SCAN,			//on a scanné l'apocalypse (on est passé voir si il y a quelque chose
		//APOCALYPSE_PRESENT,			//on a trouvé l'apocalypse
		APOCALYPSE_TAKEN,			//on a pris l'apocalypse
		APOCALYPSE_SECOND_PART,
		APOCALYPSE_WTF,
		APOCALYPSE_TOTAL_DUNE,
		APOCALYPSE_ABSENT,

		//Flags de dépose des blocs de sables
		DEPOSE_BLOC_ERROR,			//on est dans le cas d'erreur de la depose (si on a pas pu pousser le bloc precedent)
		DEPOSE_BLOC_DOUBLE_ERROR,	//on a pas pu pousser deux fois de suite
		BLOC_IN_ZONE,				//indique si il y a dejà un bloc de déposé dans la zone de construction
		DEPOSE_POS_0,
		DEPOSE_POS_1,
		DEPOSE_POS_2,
		DEPOSE_POS_3,

		//Flags de stratégie échangées entre les deux robots
		BLACK_LET_PEARL_GO_TAKE_DUNE_BLOC,
		BLACK_CAN_USE_DUNIX,
		BLACK_TRY_DOORS,

		//Flags de subaction
		BLACK_DOORS,
		BLACK_OUR_BLOC_DUNE,
		BLACK_IN_APOCALYPSE,
		BLACK_ADV_BLOC_DUNE,
		BLACK_OUR_START_ZONE,
		BLACK_DEPOSE_SAND,
		BLACK_ADV_START_ZONE,
		BLACK_DEPOSE_SEASHELL,
		BLACK_PUSH_SEASHELL,
		BLACK_IN_SNOWPLOW,
		BLACK_FISH,
		BLACK_CATCH_SEASHELL,

		PEARL_CAN_START_MATCH,
		PEARL_DOORS,
		PEARL_OUR_BLOC_DUNE,
		PEARL_ADV_BLOC_DUNE,
		PEARL_OUR_START_ZONE,
		PEARL_DEPOSE_SAND,
		PEARL_ADV_START_ZONE,
		PEARL_DEPOSE_SEASHELL,
		PEARL_OUR_SEASHELL,
		PEARL_ADV_SEASHELL,
		PEARL_CAN_GO_CLOSE_DOORS,
		PEARL_FISH,

		ELEMENTS_FLAGS_END_SYNCH,	//Les flags au-dessus seront synchro entre les deux robots

		//Flags pour savoir si la comm passe entre les deux robots
		//Ce flag est envoyé à intervalle de temps régulier
		//Ne pas mettre ce flag dans la partie synchro, ce flag est synchro par une machine à état spécifique
		COMMUNICATION_AVAILABLE,

		//Gestion des éléments dans le robot Black
		VENTOUSE_AVANT_GAUCHE_BLACK,  //La ventouse avant gauche de Black ventouse un coquillage
		VENTOUSE_AVANT_DROITE_BLACK,  //La ventouse avant gauche de Black ventouse un coquillage
		FRONT_BLACK_USED,			  //flag ppour savoir si on a des blocs de sable à l'avant du robot: A mettre à jour !!!
		BACK_BLACK_USED,		      //flag ppour savoir si on a des blocs de sable à l'arriere du robot: A mettre à jour !!!
		DUNIX_OPEN,
		PEARL_IS_OUT,


		//Gestion des éléments dans le robot Pearl
		VENTOUSE_AVANT_GAUCHE_PEARL,
		VENTOUSE_AVANT_DROITE_PEARL,
		VENTOUSE_ARRIERE_GAUCHE_PEARL,
		VENTOUSE_ARRIERE_DROITE_PEARL,
		FRONT_PEARL_USED,               //Flag pour savoir si il y a quelque chose à l'avant de Pearl
		ARM_RIGHT_LOCK,
		ARM_LEFT_LOCK,


		ELEMENTS_FLAGS_NB
	}elements_flags_e;


void ELEMENTS_init();
void ELEMENTS_process_main();

bool_e ELEMENTS_get_flag(elements_flags_e flag_id);
void ELEMENTS_set_flag(elements_flags_e flag_id, bool_e new_state);
void ELEMENTS_inc_fishs_passage();
Uint8 ELEMENTS_get_fishs_passage();

void ELEMENTS_inc_seashell_depose();
Uint8 ELEMENTS_get_seashell_depose();

bool_e ELEMENTS_fishs_passage_completed();
error_e ELEMENTS_check_communication(CAN_msg_t * msg);

ELEMENTS_property_e COQUILLAGE_get_property(Uint8 id);
bool_e COQUILLAGE_is_mine(Uint8 id);
bool_e COQUILLAGE_is_present(Uint8 id);
Uint8 COQUILLAGES_get_config();
bool_e COQUILLAGES_is_config(COQUILLAGES_config_e user_config);
void ELEMENTS_check_configuration_coquillages(bool_e activate);
void ELEMENTS_send_config_coquillages(CAN_msg_t *msg);

#ifdef USE_SYNC_ELEMENTS
void ELEMENTS_receive_flags(CAN_msg_t* msg);
#endif

void ELEMENTS_set_middle_apocalypse(Uint16 middle);
Uint16 ELEMENTS_get_middle_apocalypse();

#endif // ELEMENTS_H
