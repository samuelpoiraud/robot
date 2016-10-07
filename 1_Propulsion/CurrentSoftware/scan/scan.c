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

static scan_data_t laser_left[2*NB_SCAN_DATA];
static scan_data_t laser_right[2*NB_SCAN_DATA];
static Uint16 index = 0;
static bool_e flag_1, flag_2;


void SCAN_init(){
	Uint16 i;

	flag_1 = FALSE;
	flag_2 = FALSE;

	index = 0;

	for(i=0; i<2*NB_SCAN_DATA; i++){
		laser_left[i].enable = FALSE;
		laser_right[i].enable = FALSE;
	}
}

static void SCAN_get_data_left(){
	GEOMETRY_point_t pos_mesure, pos_laser;
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

	laser_left[index].enable = enable;

	// On calcule et on stocke la valeur même si le capteur entre en saturation
	pos_mesure.x = robot.x + OFFSET_LENGTH_LASER_LEFT*cos4096(robot.teta) + (OFFSET_WIDTH_LASER_LEFT - value)*sin4096(robot.teta);
	pos_mesure.y = robot.y + OFFSET_LENGTH_LASER_LEFT*sin4096(robot.teta) - (OFFSET_WIDTH_LASER_LEFT - value)*cos4096(robot.teta);
	// On calcule et on stocke la position du laser (c'est à dire de début d'émission du rayon laser)
	pos_laser.x = robot.x + OFFSET_LENGTH_LASER_LEFT*cos4096(robot.teta) + OFFSET_WIDTH_LASER_LEFT*sin4096(robot.teta);
	pos_laser.y = robot.y + OFFSET_LENGTH_LASER_LEFT*sin4096(robot.teta) - OFFSET_WIDTH_LASER_LEFT*cos4096(robot.teta);

	// On stocke les valeurs
	laser_left[index].pos_mesure = pos_mesure;
	laser_left[index].pos_laser = pos_laser;
}

static void SCAN_get_data_right(){
	GEOMETRY_point_t pos_mesure, pos_laser;
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

	laser_right[index].enable = enable;

	// On calcule et on stocke la valeur même si le capteur entre en saturation
	pos_mesure.x = robot.x + OFFSET_LENGTH_LASER_RIGHT*cos4096(robot.teta) + (OFFSET_WIDTH_LASER_RIGHT + value)*sin4096(robot.teta);
	pos_mesure.y = robot.y + OFFSET_LENGTH_LASER_RIGHT*sin4096(robot.teta) - (OFFSET_WIDTH_LASER_RIGHT + value)*cos4096(robot.teta);
	// On calcule et on stocke la position du laser (c'est à dire de début d'émission du rayon laser)
	pos_laser.x = robot.x + OFFSET_LENGTH_LASER_RIGHT*cos4096(robot.teta) + OFFSET_WIDTH_LASER_RIGHT*sin4096(robot.teta);
	pos_laser.y = robot.y + OFFSET_LENGTH_LASER_RIGHT*sin4096(robot.teta) - OFFSET_WIDTH_LASER_RIGHT*cos4096(robot.teta);

	// On stocke les valeurs
	laser_right[index].pos_mesure = pos_mesure;
	laser_right[index].pos_laser = pos_laser;
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
	scan_data_t tab_treatment_left[NB_SCAN_DATA], tab_treatment_right[NB_SCAN_DATA];

	// Copie de la première partie du tableau pendant que la première partie est en train de se remplir
	if(flag_1){
		for(i=0; i<NB_SCAN_DATA; i++){
			tab_treatment_left[i].pos_mesure = laser_left[i].pos_mesure;
			tab_treatment_left[i].pos_laser = laser_left[i].pos_laser;
			tab_treatment_left[i].enable = laser_left[i].enable;
			tab_treatment_right[i].pos_mesure = laser_right[i].pos_mesure;
			tab_treatment_right[i].pos_laser = laser_right[i].pos_laser;
			tab_treatment_right[i].enable = laser_right[i].enable;
			debug_printf("l(%4d ; %4d) e= %1d  r(%4d ; %4d) e= %1d\n", laser_left[i].pos_mesure.x, laser_left[i].pos_mesure.y,laser_left[i].enable, laser_right[i].pos_mesure.x, laser_right[i].pos_mesure.y, laser_right[i].enable);
		}
		flag_1 = FALSE;
	}

	// Copie de la seconde partie du tableau pendant que la première partie est en train de se remplir
	if(flag_2){
		for(i=NB_SCAN_DATA; i<2*NB_SCAN_DATA; i++){
			tab_treatment_left[i].pos_mesure = laser_left[NB_SCAN_DATA + i].pos_mesure;
			tab_treatment_left[i].pos_laser = laser_left[NB_SCAN_DATA + i].pos_laser;
			tab_treatment_left[i].enable = laser_left[NB_SCAN_DATA + i].enable;
			tab_treatment_right[i].pos_mesure = laser_right[NB_SCAN_DATA + i].pos_mesure;
			tab_treatment_right[i].pos_laser = laser_right[NB_SCAN_DATA + i].pos_laser;
			tab_treatment_right[i].enable = laser_right[NB_SCAN_DATA + i].enable;
			debug_printf("l(%4d ; %4d) e= %1d  r(%4d ; %4d) e= %1d\n", laser_left[i].pos_mesure.x, laser_left[i].pos_mesure.y,laser_left[i].enable, laser_right[i].pos_mesure.x, laser_right[i].pos_mesure.y, laser_right[i].enable);
		}
		flag_2 = FALSE;
	}

}
