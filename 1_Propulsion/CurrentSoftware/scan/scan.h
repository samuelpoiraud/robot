
#ifndef SCAN_H
#define SCAN_H
#include "../QS/QS_all.h"
#include "../QS/QS_maths.h"
#include "../QS/QS_types.h"

#define NB_SCAN_DATA (20)

typedef struct{
	 GEOMETRY_point_t pos_mesure;
	 GEOMETRY_point_t pos_laser;
     position_t pos_robot;
	 bool_e enable;
     Uint16 ADCvalue;
}scan_data_t;


void SCAN_init();
void TELEMETER_process_it();
void SCAN_process_it();
void SCAN_process_main();

Uint16 TELEMETER_get_ADCvalue_left();
Uint16 TELEMETER_get_ADCvalue_right();

#endif /* SCAN_H */
