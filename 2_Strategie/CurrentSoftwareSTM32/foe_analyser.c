#include "foe_analyser.h"
#include "avoidance.h"
#include "elements.h"

void FOE_ANALYSER_init(){

}

void FOE_ANALYSER_process_main(){
#ifdef USE_FOE_ANALYSER
	if(global.flags.match_started && ELEMENTS_get_flag(F_ADV_START_ZONE_BLOC_TAKEN) == FALSE && foe_in_square(FALSE, 700, 1100, COLOR_Y(2150), COLOR_Y(2550), FOE_TYPE_HOKUYO)){
		ELEMENTS_set_flag(F_ADV_START_ZONE_BLOC_TAKEN, TRUE);
	}
#endif
}
