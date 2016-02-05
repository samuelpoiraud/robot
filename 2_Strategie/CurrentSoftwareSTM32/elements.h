#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "QS/QS_all.h"
#include "Supervision/SD/SD.h"
#include "avoidance.h"


	typedef enum{
		OUR_ELEMENT,
		ADV_ELEMENT
	}ELEMENTS_property_e;

	typedef enum	{
		//El�ments pris
		FIRST_DOOR_CLOSED,
		SECOND_DOOR_CLOSED,
		OUR_START_ZONE_BLOC_TAKEN,		//on a pris notre bloc de 4
		ADV_START_ZONE_BLOC_TAKEN,		//on a pris le bloc de 4 adv
		OUR_DUNE_BLOC_TAKEN,			//on a pris notre bloc de 8
		ADV_DUNE_BLOC_TAKEN,			//on a pris le bloc de 8 adv
		FISHS_TAKEN,				    //poisson embarqu�s dans le robot
		DUNE_TAKEN,					    //on a pris la dune

		//Apocalypse
		OUR_BLOC_PRESENT,			//on a vu (au scan) notre bloc de 8
		ADV_BLOC_PRESENT,			//on a vu (au scan) le bloc de 8 adv
		APOCALYPSE_SCAN,			//on a scann� l'apocalypse (on est pass� voir si il y a quelque chose
		APOCALYPSE_PRESENT,			//on a trouv� l'apocalypse
		APOCALYPSE_TAKEN,			//on a pris l'apocalypse

		//Flags de d�pose des blocs de sables
		DEPOSE_BLOC_ERROR,			//on est dans le cas d'erreur de la depose (si on a pas pu pousser le bloc precedent)
		DEPOSE_BLOC_DOUBLE_ERROR,	//on a pas pu pousser deux fois de suite
		BLOC_IN_ZONE,				//indique si il y a dej� un bloc de d�pos� dans la zone de construction

		//Flags de strat�gie ou pour donner des ordres � l'autre robot
		BLACK_LET_PEARL_GO_TAKE_DUNE_BLOC,

		//� supprimer
		SEASHELLS_BOTTOM,			//si les deux coquillages du bas sont pris ou pas (flag � supprimer et � remplacer par les flag coquillages)
		OUR_BLOCK8_DEPOSED,			//on a depose notre bloc de 8 (� supprimer, on doit juste savoir si le cube est pris)
		ADV_BLOCK8_DEPOSED,			//on a depose le bloc de 8 adv (� supprimer, on doit juste savoir si le cube est pris)


		//Flags coquillages
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
		COQUILLAGES_ATTAQUE,
		COQUILLAGES_DEFENSE,

		ELEMENTS_FLAGS_END_SYNCH,	//Les flags au-dessus seront synchro entre les deux robots

		//Flags pour savoir si la comm passe entre les deux robots
		//Ce flag est envoy� � intervalle de temps r�gulier
		//Ne pas mettre ce flag dans la partie synchro, ce flag est synchro par une machine � �tat sp�cifique
		COMMUNICATION_AVAILABLE,

		//Gestion des �l�ments dans le robot Black
		VENTOUSE_AVANT_GAUCHE_BLACK,  //La ventouse avant gauche de Black ventouse un coquillage
		VENTOUSE_AVANT_DROITE_BLACK,  //La ventouse avant gauche de Black ventouse un coquillage
		FRONT_BLACK_USED,			  //flag ppour savoir si on a des blocs de sable � l'avant du robot: A mettre � jour !!!
		BACK_BLACK_USED,		      //flag ppour savoir si on a des blocs de sable � l'arriere du robot: A mettre � jour !!!

		//Gestion des �l�ments dans le robot Pearl
		VENTOUSE_AVANT_GAUCHE_PEARL,
		VENTOUSE_AVANT_DROITE_PEARL,
		VENTOUSE_ARRIERE_GAUCHE_PEARL,
		VENTOUSE_ARRIERE_DROITE_PEARL,
		FRONT_PEARL_USED,               //Flag pour savoir si il y a quelque chose � l'avant de Pearl

		ELEMENTS_FLAGS_NB
	}elements_flags_e;


void ELEMENTS_init();
void COQUILLAGE_init();

bool_e ELEMENTS_get_flag(elements_flags_e flag_id);
void ELEMENTS_set_flag(elements_flags_e flag_id, bool_e new_state);
void ELEMENTS_inc_fishs_passage();
bool_e ELEMENTS_fishs_passage_completed();
error_e ELEMENTS_check_communication(CAN_msg_t * msg);

#ifdef USE_SYNC_ELEMENTS
void ELEMENTS_receive_flags(CAN_msg_t* msg);
#endif


#endif // ELEMENTS_H
