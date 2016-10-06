#include "scan.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_adc.h"


#define CONVERSION_LASER_LEFT(x)	((Sint32)(-264*(x)+35350)/1000)
#define OFFSET_WIDTH_LASER_LEFT		(-150)
#define OFFSET_LENGTH_LASER_LEFT	(70)

#define CONVERSION_LASER_RIGHT(x)	((Sint32)(-270*(x)+35580)/1000)
#define OFFSET_WIDTH_LASER_RIGHT	(150)
#define OFFSET_LENGTH_LASER_RIGHT	(70)

#define SATURATION_LOW (300)
#define SATURATION_HIGH (1000)

static void SCAN_get_data_left();
static void SCAN_get_data_right();

typedef struct{
	 GEOMETRY_point_t data[2*NB_SCAN_DATA];
	 bool_e enable[2*NB_SCAN_DATA];
}scan_data_t;

static scan_data_t laser_left;
static scan_data_t laser_right;
static Uint16 index = 0;
static bool_e flag_1, flag_2;


void SCAN_init(){
	Uint16 i;

	flag_1 = FALSE;
	flag_2 = FALSE;

	index = 0;

	for(i=0; i<2*NB_SCAN_DATA; i++){
		laser_left.enable[i] = FALSE;
		laser_right.enable[i] = FALSE;
	}
}

static void SCAN_get_data_left(){
	GEOMETRY_point_t p;
	bool_e enable;
	Sint16 value;
	position_t robot;

	value = CONVERSION_LASER_LEFT(ADC_getValue(ADC_SENSOR_LASER_LEFT));
	robot = global.position; // On récupère la position du robot tout de suite

	// On regarde si il y a des saturations
	enable = TRUE;
	if(value < SATURATION_LOW || value > SATURATION_HIGH){
		enable = FALSE;
	}

	laser_left.enable[index] = enable;

	if(enable){
		p.x = robot.x + OFFSET_LENGTH_LASER_LEFT*cos4096(robot.teta) + OFFSET_WIDTH_LASER_LEFT*sin4096(robot.teta);
		p.y = robot.y + OFFSET_LENGTH_LASER_LEFT*sin4096(robot.teta) - OFFSET_WIDTH_LASER_LEFT*cos4096(robot.teta);

		laser_left.data[index] = p; // On stocke la valeur
	}
}

static void SCAN_get_data_right(){
	GEOMETRY_point_t p;
	bool_e enable;
	Sint16 value;
	position_t robot;

	value = CONVERSION_LASER_RIGHT(ADC_getValue(ADC_SENSOR_LASER_RIGHT));
	robot = global.position; // On récupère la position du robot tout de suite

	// On regarde si il y a des saturations
	enable = TRUE;
	if(value < SATURATION_LOW || value > SATURATION_HIGH){
		enable = FALSE;
	}

	laser_right.enable[index] = enable;


	if(enable){
		p.x = robot.x + OFFSET_LENGTH_LASER_RIGHT*cos4096(robot.teta) + OFFSET_WIDTH_LASER_RIGHT*sin4096(robot.teta);
		p.y = robot.y + OFFSET_LENGTH_LASER_RIGHT*sin4096(robot.teta) - OFFSET_WIDTH_LASER_RIGHT*cos4096(robot.teta);

		laser_right.data[index] = p; // On stocke la valeur
	}
}


void SCAN_process_it(){

	if((index < NB_SCAN_DATA && !flag_1) || (index >= NB_SCAN_DATA && !flag_2)){
		SCAN_get_data_left();
		SCAN_get_data_right();
	}else{
		debug_printf("Attention problème d'accés conccurents sur le scan laser, augmenter le nombre de données ou diminuer la fréquence d'échantillonnage\n");
	}

	index = (index + 1) % (2*NB_SCAN_DATA);

	if(index == 0)
		flag_2 = TRUE;
	else if(index == NB_SCAN_DATA)
		flag_1 = TRUE;

}

void SCAN_process_main(){
	Uint16 i;
	scan_data_treatment_t tab_treatment_left, tab_treatment_right;

	// Copie de la première partie du tableau pendant que la première partie est en train de se remplir
	if(flag_1){
		for(i=0; i<NB_SCAN_DATA; i++){
			tab_treatment_left.pos[i] = laser_left.data[i];
			tab_treatment_left.enable[i] = laser_left.enable[i];
			tab_treatment_right.pos[i] = laser_right.data[i];
			tab_treatment_right.enable[i] = laser_right.enable[i];
			debug_printf("l(%4d ; %4d) e= %1d  r(%4d ; %4d) e= %1d\n", laser_left.data[i].x, laser_left.data[i].y,laser_left.enable[i], laser_right.data[i].x, laser_right.data[i].y, laser_right.enable[i]);
		}
		flag_1 = FALSE;
	}

	// Copie de la seconde partie du tableau pendant que la première partie est en train de se remplir
	if(flag_2){
		for(i=NB_SCAN_DATA; i<2*NB_SCAN_DATA; i++){
			tab_treatment_left.pos[i] = laser_left.data[NB_SCAN_DATA + i];
			tab_treatment_left.enable[i] = laser_left.enable[NB_SCAN_DATA + i];
			tab_treatment_right.pos[i] = laser_right.data[NB_SCAN_DATA + i];
			tab_treatment_right.enable[i] = laser_right.enable[NB_SCAN_DATA + i];
			debug_printf("l(%4d ; %4d) e= %1d  r(%4d ; %4d) e= %1d\n", laser_left.data[i].x, laser_left.data[i].y,laser_left.enable[i], laser_right.data[i].x, laser_right.data[i].y, laser_right.enable[i]);
		}
		flag_2 = FALSE;
	}

}
