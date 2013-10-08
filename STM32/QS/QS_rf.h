#ifndef QS_RF_H
#define QS_RF_H

#include "QS_types.h"

void RF_init();

void RF_set_channel(Uint8 channel);
void RF_set_output_power(Uint8 power);
void RF_set_destination_id(Uint8 address);

Uint8 RF_get_channel();
Uint8 RF_get_output_power();
Uint8 RF_get_rssi();
Uint8 RF_get_destination_id();

#endif // QS_RF_H
