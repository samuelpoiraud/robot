#include "scan.h"

#ifdef SCAN

#include "../QS/QS_outputlog.h"
#include "../QS/QS_adc.h"
#include "../QS/QS_ads1118.h"

#include "objects_scan.h"
#include "borders_scan.h"

#define CONVERSION_LASER_LEFT(x)	((Sint32)(36148*(x)-6611800)/10000)//((Sint32)(37217*(x)-7096800)/10000)
#define OFFSET_WIDTH_LASER_LEFT		(144)
#define OFFSET_LENGTH_LASER_LEFT	(80)
#define OFFSET_ANGLE_LEFT           -102

#define CONVERSION_LASER_RIGHT(x)	((Sint32)(1413*(x)-6020000)/10000)//((Sint32)(36130*(x)-6247900)/10000)
#define OFFSET_WIDTH_LASER_RIGHT	(144)
#define OFFSET_LENGTH_LASER_RIGHT	(80)
#define OFFSET_ANGLE_RIGHT          39


#define SATURATION_LOW (200)
#define SATURATION_HIGH (1000)

typedef enum{
	SCAN_SIDE_LEFT,
	SCAN_SIDE_RIGHT
}SCAN_side_e;

//static void SCAN_get_data_left();
//static void SCAN_get_data_right();

static volatile scan_data_t laser_left[2*NB_SCAN_DATA];
static volatile scan_data_t laser_right[2*NB_SCAN_DATA];
static volatile Uint16 index = 0;
static volatile bool_e flag_1, flag_2, flag_treatment_too_slow;
//static scan_data_t tab_treatment[NB_SCAN_DATA];

static GEOMETRY_point_t pos_mesure;

#ifdef USE_ADS1118_ON_ADC
	static void SCAN_adcConfigInit();
	static ADS1118_sensorConfig_t sensorLeftConfig, sensorRightConfig;
#endif

void SCAN_init(){
	Uint16 i;

	flag_1 = FALSE;
	flag_2 = FALSE;
	flag_treatment_too_slow = FALSE;

	index = 0;

	for(i=0; i<2*NB_SCAN_DATA; i++){
		laser_left[i].enable = FALSE;
		laser_right[i].enable = FALSE;
	}

	#ifdef USE_ADS1118_ON_ADC
		SCAN_adcConfigInit();
	#endif
}

#ifdef USE_ADS1118_ON_ADC
	static void SCAN_adcConfigInit(){
		sensorRightConfig.GPIO_cs.GPIOx = GPIOC;
		sensorRightConfig.GPIO_cs.GPIO_Pin = GPIO_Pin_2;
		sensorRightConfig.GPIO_din.GPIOx = GPIOB;
		sensorRightConfig.GPIO_din.GPIO_Pin = GPIO_Pin_14;
		sensorRightConfig.SPI_handle = SPI2;

		ADS1118_init(&sensorRightConfig);

		bool_e res;
		res = ADS1118_setConfig(&sensorRightConfig,
				ADS1118_CONFIG_SINGLE_SHOT_OFF,
				ADS1118_CONFIG_INPUT_MULTIPLEXER__AINP_IS_AIN0__AINN_IS_GND,
				ADS1118_CONFIG_GAIN_AMPLIFIER_FSR_4_096,
				ADS1118_CONFIG_MODE_CONTINUOUS_CONVERSION,
				ADS1118_CONFIG_DATA_RATE_860_SPS,
				ADS1118_CONFIG_SENSOR_MODE_ADC,
				ADS1118_CONFIG_PULLUP_ENABLE,
				TRUE);
		if(res)
			debug_printf("Configuration de l'ADC ADS1118 r�ussie\n");
		else
			debug_printf("Configuration de l'ADC ADS1118 �chou� !!!!!!!!!!!!!!\n");

	}
#endif

void SCAN_onPower(){
	#ifdef USE_ADS1118_ON_ADC
		SCAN_adcConfigInit();
	#endif
}

static void SCAN_get_data(SCAN_side_e side){

	// Variable static
	static scan_zone_e previous_zone_left = OTHER_ZONE;
	static scan_zone_e zone_left = OTHER_ZONE;
	static GEOMETRY_point_t previous_pos_left = {0,0};
	static scan_zone_e previous_zone_right = OTHER_ZONE;
	static scan_zone_e zone_right = OTHER_ZONE;
	static GEOMETRY_point_t previous_pos_right = {0,0};

	GEOMETRY_point_t pos_laser;
	bool_e enable;
	Sint16 value;
	position_t robot;
	Sint16 cosinus1 = 0, sinus1 = 0, cosinus2 = 0, sinus2 = 0;
	//Sint32 offsetLenghtLaser, offsetWidthLaser
	Sint32 offsetAngle;
	scan_zone_e *previous_zone;
	scan_zone_e *zone;
	GEOMETRY_point_t *previous_pos;
	volatile scan_data_t *laser_data;

	//IT_printf("essayeencore\n");
	if(side == SCAN_SIDE_LEFT){
		//offsetLenghtLaser = OFFSET_LENGTH_LASER_LEFT;
		//offsetWidthLaser = OFFSET_WIDTH_LASER_LEFT;
		offsetAngle = OFFSET_ANGLE_LEFT;
		previous_zone = &previous_zone_left;
		zone = &zone_left;
		previous_pos = &previous_pos_left;
		laser_data = laser_left;

		value = CONVERSION_LASER_LEFT(laser_data[index].ADCvalue);

	}else{
		//offsetLenghtLaser = OFFSET_LENGTH_LASER_RIGHT;
		//offsetWidthLaser = OFFSET_WIDTH_LASER_RIGHT;
		offsetAngle = OFFSET_ANGLE_RIGHT;
		previous_zone = &previous_zone_right;
		zone = &zone_right;
		previous_pos = &previous_pos_right;
		laser_data = laser_right;

		value = CONVERSION_LASER_RIGHT(laser_data[index].ADCvalue);
	}

	robot = global.position; // On r�cup�re la position du robot tout de suite

	robot.teta = GEOMETRY_modulo_angle(robot.teta);
	COS_SIN_4096_get(robot.teta, &cosinus1, &sinus1);

	// On regarde si il y a des saturations
	enable = TRUE;
	display(laser_data[index].ADCvalue);
//	if(laser_data[index].ADCvalue < SATURATION_LOW || laser_data[index].ADCvalue > SATURATION_HIGH){
	//	enable = FALSE;
	//}
	laser_data[index].enable = enable;

	// On calcule et on stocke la position du laser (c'est � dire de d�but d'�mission du rayon laser)
	if(side == SCAN_SIDE_LEFT){
		pos_laser.x = robot.x + (OFFSET_LENGTH_LASER_LEFT*cosinus1 - OFFSET_WIDTH_LASER_LEFT*sinus1)/4096;
		pos_laser.y = robot.y + (OFFSET_LENGTH_LASER_LEFT*sinus1 + OFFSET_WIDTH_LASER_LEFT*cosinus1)/4096;

	}else{
		pos_laser.x = robot.x + (OFFSET_LENGTH_LASER_RIGHT*cosinus1 + OFFSET_WIDTH_LASER_RIGHT*sinus1)/4096;
		pos_laser.y = robot.y + (OFFSET_LENGTH_LASER_RIGHT*sinus1 - OFFSET_WIDTH_LASER_RIGHT*cosinus1)/4096;
	}

	COS_SIN_4096_get(robot.teta + offsetAngle, &cosinus2, &sinus2);

	// On calcule et on stocke la valeur sauf si le capteur entre en saturation
	if(enable == TRUE){
		if(side == SCAN_SIDE_LEFT){
			pos_mesure.x = robot.x + ((OFFSET_LENGTH_LASER_LEFT*cosinus1 - OFFSET_WIDTH_LASER_LEFT*sinus1) - (value * sinus2))/4096;
			pos_mesure.y = robot.y + ((OFFSET_LENGTH_LASER_LEFT*sinus1 + OFFSET_WIDTH_LASER_LEFT*cosinus1) + (value * cosinus2))/4096;
			//display(pos_mesure.x);
			//display(pos_mesure.y);
		}else{
			pos_mesure.x = robot.x + ((OFFSET_LENGTH_LASER_RIGHT*cosinus1 + OFFSET_WIDTH_LASER_RIGHT*sinus1) + (value * sinus2))/4096;
			pos_mesure.y = robot.y + ((OFFSET_LENGTH_LASER_RIGHT*sinus1 - OFFSET_WIDTH_LASER_RIGHT*cosinus1) - (value * cosinus2))/4096;
			display(pos_mesure.x);
			display(pos_mesure.y);
		}


		if(absolute(previous_pos->x - pos_mesure.x) > 15 || absolute(previous_pos->y - pos_mesure.y) > 15)
		{
			*previous_zone = *zone;
			*zone = BORDERS_SCAN_treatment(pos_mesure);
			*previous_pos = pos_mesure;
			if(*previous_zone != *zone){
				BORDERS_SCAN_calculeZonePublic(*previous_zone);
			}
		}
	}
	// On stocke les valeurs
	laser_data[index].pos_mesure = pos_mesure;
	laser_data[index].pos_laser = pos_laser;
	//laser_data[index].ADCvalue = value;
	laser_data[index].pos_robot = robot;

}

/*static void SCAN_get_data_left(){
	GEOMETRY_point_t pos_mesure, pos_laser;
	bool_e enable;
	Sint16 value;
	position_t robot;
	Sint16 cosinus = 0, sinus = 0;

	static scan_zone_e previous_zone = {0};
	static scan_zone_e zone = {0};
	static GEOMETRY_point_t previous_pos = {0,0};

	robot = global.position; // On r�cup�re la position du robot tout de suite
	//printf("%d\n",value);
	value = CONVERSION_LASER_LEFT(laser_left[index].ADCvalue);

	//robot = global.position; // On r�cup�re la position du robot tout de suite
	robot.teta = GEOMETRY_modulo_angle(robot.teta);
	COS_SIN_4096_get(robot.teta, &cosinus, &sinus);

	// On regarde si il y a des saturations
	enable = TRUE;
	if(laser_left[index].ADCvalue < SATURATION_LOW || laser_left[index].ADCvalue > SATURATION_HIGH){
		enable = FALSE;
	}
	laser_left[index].enable = enable;

	// On calcule et on stocke la position du laser (c'est � dire de d�but d'�mission du rayon laser)
	pos_laser.x = robot.x + (OFFSET_LENGTH_LASER_LEFT*cosinus - OFFSET_WIDTH_LASER_LEFT*sinus)/4096;
	pos_laser.y = robot.y + (OFFSET_LENGTH_LASER_LEFT*sinus + OFFSET_WIDTH_LASER_LEFT*cosinus)/4096;

	COS_SIN_4096_get(robot.teta+OFFSET_ANGLE_LEFT, &cosinus, &sinus);

	// On calcule et on stocke la valeur sauf si le capteur entre en saturation
	if(enable==TRUE){
		pos_mesure.x = pos_laser.x - (value * sinus)/4096;
		//it_printf("pos_mes:%d\tpos_las:%d\tvalue:%d\n",pos_mesure.x,pos_laser.x,value);

		pos_mesure.y = pos_laser.y + (value * cosinus)/4096;

		if(absolute(previous_pos.x - pos_mesure.x) > 15 || absolute(previous_pos.y - pos_mesure.y) > 15)
		{
			previous_zone = zone;
			zone = BORDERS_SCAN_treatment(pos_mesure);
			previous_pos = pos_mesure;
			if(previous_zone != zone){
				//it_printf("j\n");
				calculeZonePublic(previous_zone);
			}
		}
	}

	// On stocke les valeurs
	laser_left[index].pos_mesure = pos_mesure;
	laser_left[index].pos_laser = pos_laser;
	//laser_left[index].ADCvalue = value;
	laser_left[index].pos_robot = robot;
}

static void SCAN_get_data_right(){
	GEOMETRY_point_t pos_mesure, pos_laser;
	bool_e enable;
	Sint16 valueADC, value;
	position_t robot;
	Sint16 cosinus = 0, sinus = 0;

  //  valueADC = ADC_getValue(ADC_SENSOR_LASER_RIGHT);
    value = CONVERSION_LASER_RIGHT(laser_right[index].ADCvalue);
   // it_printf("%d\n",value);

	robot = global.position; // On r�cup�re la position du robot tout de suite
	robot.teta = GEOMETRY_modulo_angle(robot.teta);
	COS_SIN_4096_get(robot.teta, &cosinus, &sinus);

	// On regarde si il y a des saturations
	enable = TRUE;
#warning Ici on doit regarder la saturation de l ADC donc value doit �tre la valeur en sortie de l ADC et pas une distance
	if(value < SATURATION_LOW || value > SATURATION_HIGH){
		enable = FALSE;
	}

	laser_right[index].enable = enable;

	// On calcule et on stocke la valeur m�me si le capteur entre en saturation
	pos_mesure.x = (Sint16) (robot.x + (OFFSET_LENGTH_LASER_RIGHT*cosinus + (OFFSET_WIDTH_LASER_RIGHT + value)*sinus))/4096.0;
	pos_mesure.y = (Sint16) (robot.y + (OFFSET_LENGTH_LASER_RIGHT*sinus - (OFFSET_WIDTH_LASER_RIGHT + value)*cosinus))/4096.0;
	// On calcule et on stocke la position du laser (c'est � dire de d�but d'�mission du rayon laser)
	pos_laser.x = (Sint16) (robot.x + (OFFSET_LENGTH_LASER_RIGHT*cosinus + OFFSET_WIDTH_LASER_RIGHT*sinus))/4096.0;
	pos_laser.y = (Sint16) (robot.y + (OFFSET_LENGTH_LASER_RIGHT*sinus - OFFSET_WIDTH_LASER_RIGHT*cosinus))/4096.0;

    COS_SIN_4096_get(robot.teta+OFFSET_ANGLE_RIGHT, &cosinus, &sinus);
    pos_mesure.x=pos_laser.x+(value) * cosinus/4096;
    pos_mesure.y=pos_laser.y+(value) * sinus/4096;


	// On stocke les valeurs
	laser_right[index].pos_mesure = pos_mesure;
	laser_right[index].pos_laser = pos_laser;
    laser_right[index].ADCvalue = valueADC;
}*/


void TELEMETER_process_it(){
	
	laser_left[index].ADCvalue = ADC_getValue(ADC_SENSOR_LASER_LEFT);
#ifdef USE_ADS1118_ON_ADC
	ADS1118_getValue(&sensorRightConfig, (Uint16 *) &(laser_right[index].ADCvalue));
//	it_printf("ADCSPI: %d\n", laser_right[index].ADCvalue);
#else
	laser_right[index].ADCvalue = ADC_getValue(ADC_SENSOR_LASER_RIGHT);
#endif

	//printf("%d\n",laser_right[index].ADCvalue);
}


Uint16 TELEMETER_get_ADCvalue_left(){
	//display(laser_left[index].ADCvalue);
	return laser_left[index].ADCvalue;
}

Uint16 TELEMETER_get_ADCvalue_right(){
	return laser_right[index].ADCvalue;
}

GEOMETRY_point_t SCAN_get_pos_mesure(){
	return pos_mesure;
}

void SCAN_process_it(){
	if((index < NB_SCAN_DATA && !flag_1) || (index >= NB_SCAN_DATA && !flag_2)){
		SCAN_get_data(SCAN_SIDE_LEFT);
		SCAN_get_data(SCAN_SIDE_RIGHT);
		//SCAN_get_data(SCAN_SIDE_LEFT);

//		SCAN_get_data_left();
//		SCAN_get_data_right();
	}else{
		flag_treatment_too_slow = TRUE;
	}

	index = (index + 1) % (2 * NB_SCAN_DATA);

	if(index == 0)
		flag_2 = TRUE;
	else if(index == NB_SCAN_DATA)
		flag_1 = TRUE;

}

void SCAN_process_main(){

	Uint16 i;
	scan_data_t tab_treatment_left[NB_SCAN_DATA], tab_treatment_right[NB_SCAN_DATA];

	if(flag_treatment_too_slow){
	//	debug_printf("probl�me d'acc�s conccurents\n");
	}
	Uint32 moy=0;
	// Copie de la premi�re partie du tableau pendant que la premi�re partie est en train de se remplir
	if(flag_1){
		//debug_printf("\n\n VIDAGE FLAG_1\n");
		for(i=0; i<NB_SCAN_DATA; i++){
			//printf("%d\n",laser_right[i].ADCvalue);
			tab_treatment_left[i].pos_mesure = laser_left[i].pos_mesure;
			tab_treatment_left[i].pos_laser = laser_left[i].pos_laser;
			tab_treatment_left[i].enable = laser_left[i].enable;
			tab_treatment_right[i].pos_mesure = laser_right[i].pos_mesure;
			tab_treatment_right[i].pos_laser = laser_right[i].pos_laser;
			tab_treatment_right[i].enable = laser_right[i].enable;
			moy+=laser_right[i].ADCvalue;
			//debug_printf("i1=%d\n", i);
			//debug_printf("i1=%d l(%4d ; %4d) e= %1d  r(%4d ; %4d) e= %1d\n", i, laser_left[i].pos_mesure.x, laser_left[i].pos_mesure.y,laser_left[i].enable, laser_right[i].pos_mesure.x, laser_right[i].pos_mesure.y, laser_right[i].enable);
		}
		flag_1 = FALSE;
		//printf("%ld\n",moy);
		moy=moy/NB_SCAN_DATA;
		//printf("\n\n%ld\n\n\n",moy);

		// Appel de fonctions de scan objets
		//OBJECTS_SCAN_treatment(tab_treatment_left);
		//OBJECTS_SCAN_treatment(tab_treatment_right);
		// BORDERS_SCAN_treatment(tab_treatment_left);
		// BORDERS_SCAN_treatment(tab_treatment_right);
	}

	// Copie de la seconde partie du tableau pendant que la premi�re partie est en train de se remplir
	if(flag_2){
		//debug_printf("\n\n VIDAGE FLAG_2\n");
		for(i=0; i<NB_SCAN_DATA; i++){
			tab_treatment_left[i].pos_mesure = laser_left[NB_SCAN_DATA + i].pos_mesure;
			tab_treatment_left[i].pos_laser = laser_left[NB_SCAN_DATA + i].pos_laser;
			tab_treatment_left[i].enable = laser_left[NB_SCAN_DATA + i].enable;
			tab_treatment_right[i].pos_mesure = laser_right[NB_SCAN_DATA + i].pos_mesure;
			tab_treatment_right[i].pos_laser = laser_right[NB_SCAN_DATA + i].pos_laser;
			tab_treatment_right[i].enable = laser_right[NB_SCAN_DATA + i].enable;
			moy+=laser_right[i].ADCvalue;
			//debug_printf("i2=%d\n", i);
			//debug_printf("i2=%d l(%4d ; %4d) e= %1d  r(%4d ; %4d) e= %1d\n", i, laser_left[i].pos_mesure.x, laser_left[i].pos_mesure.y,laser_left[i].enable, laser_right[i].pos_mesure.x, laser_right[i].pos_mesure.y, laser_right[i].enable);
		}
		flag_2 = FALSE;
		moy=moy/NB_SCAN_DATA;
		//printf("%ld\n",moy);
		// Appel des fonctions de scans objets
		//OBJECTS_SCAN_treatment(tab_treatment_left);
		//OBJECTS_SCAN_treatment(tab_treatment_right);
	}


	/*static time32_t time = 0;
	if(global.absolute_time - time > 500){
		ADS1118_getValue(&sensorRightConfig, &(laser_right[index].ADCvalue));
		it_printf("ADCSPI: %d\n", laser_right[index].ADCvalue);
		time = global.absolute_time;
	}*/
}

#endif
