#ifndef SYNCHRO_BALISES_H
#define SYNCHRO_BALISES_H

#include "../QS/QS_all.h"
#include "../QS/QS_rf.h"

void SYNCHRO_init();
void SYNCHRO_process_main();

bool_e get_warner_foe1_is_rf_unreacheable(void);
bool_e get_warner_foe2_is_rf_unreacheable(void);
bool_e get_warner_low_battery_on_foe1(void);
bool_e get_warner_low_battery_on_foe2(void);

#endif // SYNCHRO_BALISES_H
