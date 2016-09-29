#include "actionChecker.h"
#include <stdarg.h>
#include "../QS/QS_outputlog.h"
#include "../act_can.h"

Uint8 check_act_status(queue_id_e act_queue_id, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state) {
	ACT_function_result_e result = ACT_get_last_action_result(act_queue_id);
	switch(result) {
		case ACT_FUNCTION_InProgress:
			return in_progress_state;

		case ACT_FUNCTION_Done:
			return success_state;

		case ACT_FUNCTION_ActDisabled:
		case ACT_FUNCTION_RetryLater:
			return failed_state;

		default:
			debug_printf("/!\\ check_act_status:%d: in default case, result = %d\n", __LINE__, result);
			return failed_state;
	}
}

Uint8 check_sub_action_result(error_e sub_action, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state) {
	switch(sub_action) {
		case IN_PROGRESS:
			return in_progress_state;

		case END_OK:
			return success_state;

		case END_WITH_TIMEOUT:
		case FOE_IN_PATH:
		case NOT_HANDLED:
			return failed_state;

		default:
			debug_printf("/!\\ check_sub_action_result:%d: in default case, sub_action = %d\n", __LINE__, sub_action);
			return failed_state;
	}
}
