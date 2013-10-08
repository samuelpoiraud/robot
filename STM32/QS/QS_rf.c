#include "QS_rf.h"

//config only one time, internal ram has 10k cycles ...
static void RF_config_set_channel(Uint8 channel);
static void RF_config_set_output_power(Uint8 power);
static void RF_config_set_internal_buffer_size(Uint8 size);
static void RF_config_set_buffer_timeout(Uint16 ms);
static void RF_config_set_packed_end_char(char c);
static void RF_config_set_unique_id(Uint8 id);
static void RF_config_set_system_id(Uint8 id);
static void RF_config_set_broadcast_id(Uint8 id);
static void RF_config_set_destination_id(Uint8 id);
