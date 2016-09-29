#include "QS_stateMachineHelper.h"
#include <stdarg.h>
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES
#include "../QS/QS_outputlog.h"

void UTILS_LOG_state_changed(const char* sm_name, UTILS_state_machine_id_e sm_id, const char* old_state_name, Uint8 old_state_id, const char* new_state_name, Uint8 new_state_id) {
	info_printf("  # %s(0x%04X): %s(%d) -> %s(%d)\n",
			sm_name, sm_id,
			old_state_name, old_state_id,
			new_state_name, new_state_id);
}

void UTILS_LOG_init_state(const char* sm_name, UTILS_state_machine_id_e sm_id, const char* init_state_name, Uint8 init_state_val){
	info_printf("  # %s(0x%04X): %s\n",
			sm_name, sm_id,
			init_state_name);
}
