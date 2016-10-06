
#include "../QS/QS_all.h"
#include "../QS/QS_maths.h"
#include "../QS/QS_types.h"

#define NB_SCAN_DATA (500)

typedef struct{
	 GEOMETRY_point_t pos[NB_SCAN_DATA];
	 bool_e enable[NB_SCAN_DATA];
}scan_data_treatment_t;

void SCAN_init();
void SCAN_process_it();
void SCAN_process_main();
