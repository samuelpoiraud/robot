#include "elements.h"

#define LOG_PREFIX "element: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ELEMENTS
#include "QS/QS_outputlog.h"


volatile ELEMENTS_element_t ELEMENTS_spot[NB_SPOT];
volatile ELEMENTS_element_t ELEMENTS_cup[NB_CUP];
volatile bool_e dispensers_done[NB_DISPENSERS];

volatile Uint8 holly_left_spot_level;	//Nb de pied dans un spot. La balle ne compte pas.
volatile Uint8 holly_right_spot_level;	//Nb de pied dans un spot. La balle ne compte pas.

volatile bool_e elements_flags[ELEMENTS_FLAGS_NB];

ELEMENTS_position cup[3];
Uint8 nb_cup=0;
bool_e end_transmission_cup=FALSE;


void ELEMENTS_init(){
	Uint8 i;

	holly_left_spot_level = 0;	//Nb de pied dans un spot. La balle ne compte pas.
	holly_right_spot_level = 0;	//Nb de pied dans un spot. La balle ne compte pas.

	for(i=0;i<ELEMENTS_FLAGS_NB;i++)
		elements_flags[i] = FALSE;

	for(i=0;i<NB_SPOT;i++)
		ELEMENTS_spot[i].state = AVAILABLE;

	// Spot Jaune
	ELEMENTS_spot[0].color = YELLOW;
	ELEMENTS_spot[0].x = 200;
	ELEMENTS_spot[0].y = 50;

	ELEMENTS_spot[1].color = YELLOW;
	ELEMENTS_spot[1].x = 100;
	ELEMENTS_spot[1].y = 850;

	ELEMENTS_spot[2].color = YELLOW;
	ELEMENTS_spot[2].x = 200;
	ELEMENTS_spot[2].y = 850;

	ELEMENTS_spot[3].color = YELLOW;
	ELEMENTS_spot[3].x = 1750;
	ELEMENTS_spot[3].y = 90;

	ELEMENTS_spot[4].color = YELLOW;
	ELEMENTS_spot[4].x = 1850;
	ELEMENTS_spot[4].y = 90;

	ELEMENTS_spot[5].color = YELLOW;
	ELEMENTS_spot[5].x = 1355;
	ELEMENTS_spot[5].y = 870;

	ELEMENTS_spot[6].color = YELLOW;
	ELEMENTS_spot[6].x = 1770;
	ELEMENTS_spot[6].y = 1100;

	ELEMENTS_spot[7].color = YELLOW;
	ELEMENTS_spot[7].x = 1400;
	ELEMENTS_spot[7].y = 1300;

	// Spot Vert
	ELEMENTS_spot[8].color = GREEN;
	ELEMENTS_spot[8].x = 200;
	ELEMENTS_spot[8].y = 2910;

	ELEMENTS_spot[9].color = GREEN;
	ELEMENTS_spot[9].x = 100;
	ELEMENTS_spot[9].y = 2150;

	ELEMENTS_spot[10].color = GREEN;
	ELEMENTS_spot[10].x = 200;
	ELEMENTS_spot[10].y = 2150;

	ELEMENTS_spot[11].color = GREEN;
	ELEMENTS_spot[11].x = 1850;
	ELEMENTS_spot[11].y = 2910;

	ELEMENTS_spot[12].color = GREEN;
	ELEMENTS_spot[12].x = 1750;
	ELEMENTS_spot[12].y = 2910;

	ELEMENTS_spot[13].color = GREEN;
	ELEMENTS_spot[13].x = 1355;
	ELEMENTS_spot[13].y = 2130;

	ELEMENTS_spot[14].color = GREEN;
	ELEMENTS_spot[14].x = 1770;
	ELEMENTS_spot[14].y = 1900;

	ELEMENTS_spot[15].color = GREEN;
	ELEMENTS_spot[15].x = 1400;
	ELEMENTS_spot[15].y = 1700;

	//Cup
	for(i=0;i<NB_CUP;i++)
		ELEMENTS_cup[i].state = AVAILABLE;

	ELEMENTS_cup[0].x = 830;
	ELEMENTS_cup[0].y = 910;

	ELEMENTS_cup[1].x = 1750;
	ELEMENTS_cup[1].y = 250;

	ELEMENTS_cup[2].x = 1650;
	ELEMENTS_cup[2].y = 1500;

	ELEMENTS_cup[3].x = 830;
	ELEMENTS_cup[3].y = 2090;

	ELEMENTS_cup[4].x = 1750;
	ELEMENTS_cup[4].y = 2750;
}


bool_e ELEMENTS_get_flag(elements_flags_e flag_id)
{
	assert(flag_id < ELEMENTS_FLAGS_NB);
	return elements_flags[flag_id];
}

void ELEMENTS_set_flag(elements_flags_e flag_id, bool_e new_state)
{
	assert(flag_id < ELEMENTS_FLAGS_NB);
	elements_flags[flag_id] = new_state;
}


// Récupérer l'état ET les coordonnées d'un distributeur de popcorns dont l'id est passé en paramètre.
void ELEMENTS_get_dispenser_state(elements_flags_e dispenser_id, bool_e * dispenser_done, Uint16 * x, Uint16 * y)
{
	switch(dispenser_id)
	{
		case OUR_BORDER_DISPENSER:
			*y = COLOR_Y(300);
			break;
		case OUR_STAIRS_DISPENSER:
			*y = COLOR_Y(600);
			break;
		case ADVERSARY_STAIRS_DISPENSER:
			*y = COLOR_Y(2400);
			break;
		case ADVERSARY_BORDER_DISPENSER:
			*y = COLOR_Y(2700);
			break;
		default:
			assert(0);
			return;
	}
	*dispenser_done = elements_flags[dispenser_id];
	*x = 24;
}


//Savoir le nombre de pied dans l'ascenceur gauche
Uint8 ELEMENTS_get_holly_left_spot_level(void)
{
	return holly_left_spot_level;
}

//Indiquer la récupération d'un nouveau pied dans l'ascenceur de gauche
void ELEMENTS_inc_holly_left_spot_level(void)
{
	holly_left_spot_level++;
}

//Indiquer le vidage de l'ascenceur de gauche
void ELEMENTS_reset_holly_left_spot_level(void)
{
	holly_left_spot_level = 0;
}

//Savoir le nombre de pied dans l'ascenceur droite
Uint8 ELEMENTS_get_holly_right_spot_level(void)
{
	return holly_right_spot_level;
}

//Indiquer la récupération d'un nouveau pied dans l'ascenceur de droite
void ELEMENTS_inc_holly_right_spot_level(void)
{
	holly_right_spot_level++;
}

//Indiquer le vidage de l'ascenceur de droite
void ELEMENTS_reset_holly_right_spot_level(void)
{
	holly_right_spot_level = 0;
}

//Indiquer qu'un gobelet a été pris
void ELEMENTS_set_cup(Uint8 number, ELEMENTS_state_s stateCup){
	ELEMENTS_cup[number].state = stateCup;
}

 ELEMENTS_state_s ELEMENTS_get_cup(Uint8 number){
	return ELEMENTS_cup[number].state;
}

 void collect_cup_coord(CAN_msg_t *msg){
	 if( msg->data[0] & 0x04){  //pas de gobelets de détectés
		  end_transmission_cup=TRUE;
		  nb_cup=0;
	 }else{  //si on a des gobelets de détectés
		 cup[nb_cup].x=U16FROMU8(msg->data[1],msg->data[2]);
		 cup[nb_cup].y=U16FROMU8(msg->data[3],msg->data[4]);
		 nb_cup++;
		 if(msg->data[0] & 0x01)  //dernier gobelet détecté ?
			 end_transmission_cup=TRUE;
		 else
			 end_transmission_cup=FALSE;
	 }
 }

 bool_e get_cup_transmission(void){
	 //return 1;  //test
	 return end_transmission_cup;
 }

 Uint8 get_number_cup(void){
	 //return 1;  //test
	 return nb_cup;
 }

 void copy_cup_position(ELEMENTS_position cup_copy[]){
	 Uint8 i;
	 if(nb_cup<=3){
		 for(i=0;i<nb_cup;i++){
			 cup_copy[i].x=cup[i].x;
			 cup_copy[i].y=cup[i].y;
			 cup_copy[i].state_cup=AVAILABLE;
		 }
		 for(i=nb_cup;i<3;i++){
			 cup_copy[i].state_cup=LOST;
		 }
	 }
 }


#define	TIMEOUT_PROTECT	1000
static time32_t hollyTimeProtect = 0;

void holly_send_message_protect_process_main(){
	if(global.env.match_time > (hollyTimeProtect + TIMEOUT_PROTECT)){
		hollyTimeProtect = global.env.match_time;
		holly_ask_protect_to_wood();
	}
}
