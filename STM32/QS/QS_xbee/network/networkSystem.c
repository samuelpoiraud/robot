#include "networkSystem.h"
#include "networkReceiveManager.h"
#include "networkSessionManager.h"
#include "lowLevel/networkSpreader.h"
#include "lowLevel/networkReceiver.h"
#include "../../QS_uart.h"

#ifdef USE_XBEE
	typedef enum{
		NETWORK_INIT,
		NETWORK_RESET_XBEE,
		NETWORK_WAIT_REBOOT,
		NETWORK_BOOTLOADER,
		NETWORK_READY
	}state_e;

	static volatile bool_e networkAvailable = FALSE;

	void NETWORK_SYSTEM_init(){
		NETWORK_RECEIVE_MANAGER_init();
		NETWORK_SESSION_MNG_init();
		NETWORK_RECEIVER_init();
		NETWORK_SPREADER_init();
		NETWORK_SPREADER_setUser(SYSTEM_UART_USER);
	}

	void NETWORK_SYSTEM_processMain(){
		static state_e state = NETWORK_INIT;
		static time32_t begin;

		switch(state){
			case NETWORK_INIT:
				begin = global.absolute_time;
				GPIO_SetBits(XBEE_RESET);
				state = NETWORK_RESET_XBEE;
				break;

			case NETWORK_RESET_XBEE :
				if(global.absolute_time - begin > 200){
					GPIO_ResetBits(XBEE_RESET);
					begin = global.absolute_time;
					state = NETWORK_WAIT_REBOOT;
				}
				break;

			case NETWORK_WAIT_REBOOT:
				if(global.absolute_time - begin > 5000){
					state = NETWORK_BOOTLOADER;
				}
				break;

			case NETWORK_BOOTLOADER:
				NETWORK_SPREADER_systemWrite('b');
				printf("Ready !\n");
				NETWORK_SPREADER_setUser(NETWORK_UART_USER);
				state = NETWORK_READY;
				break;

			case NETWORK_READY:
				networkAvailable = TRUE;
				break;
		}


		NETWORK_RECEIVE_MANAGER_processMain();
		NETWORK_SESSION_MNG_processMain();
	}

	bool_e NETWORK_SYSTEM_isAvailable(){
		return networkAvailable;
	}

#endif